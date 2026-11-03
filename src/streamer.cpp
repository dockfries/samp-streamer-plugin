/*
 * Copyright (C) 2017 Incognito
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "main.h"

#include "streamer.h"
#include "core.h"

Streamer::Streamer()
{
	averageElapsedTime = 0.0f;
	lastUpdateTime = 0.0f;
	tickCount = 0;
	tickRate = 50;
	hasLastUpdateTickTime = false;
	updateBudget = 1.5f;
	velocityBoundaries = std::make_tuple(0.25f, 7.5f);
}

void Streamer::calculateAverageElapsedTime()
{
	std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
	static std::chrono::steady_clock::time_point lastRecordedTime;
	static Eigen::Array<float, 5, 1> recordedTimes = Eigen::Array<float, 5, 1>::Zero();
	if (lastRecordedTime.time_since_epoch().count())
	{
		if (!(recordedTimes > 0).all())
		{
			std::chrono::duration<float> elapsedTime = currentTime - lastRecordedTime;
			recordedTimes[(recordedTimes > 0).count()] = elapsedTime.count();
		}
		else
		{
			averageElapsedTime = recordedTimes.mean() * 50.0f;
			recordedTimes.setZero();
		}
	}
	lastRecordedTime = currentTime;
}

void Streamer::startAutomaticUpdate()
{
	if (!core->getData()->interfaces.empty())
	{
		std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
		// Bound the per-player work of one tick to roughly half of the observed
		// server tick (capped at 3 ms). Without this, a tick where many players
		// are due costs players * update time and stalls the server regardless
		// of the configured tick rates.
		updateBudget = 1.5f;
		if (hasLastUpdateTickTime)
		{
			float tickInterval = std::chrono::duration<float, std::milli>(currentTime - lastUpdateTickTime).count();
			if (tickInterval > 0.0f)
			{
				updateBudget = std::max(0.5f, std::min(3.0f, tickInterval * 0.5f));
			}
		}
		lastUpdateTickTime = currentTime;
		hasLastUpdateTickTime = true;
		float spent = 0.0f;
		bool budgetExhausted = false;
		if (!core->getData()->players.empty())
		{
			// Players deferred by the previous tick are retried first so a long
			// list of due players cannot starve them.
			if (!deferredPlayers.empty())
			{
				std::vector<int> carried;
				carried.swap(deferredPlayers);
				for (std::vector<int>::const_iterator d = carried.begin(); d != carried.end(); ++d)
				{
					std::unordered_map<int, Player>::iterator p = core->getData()->players.find(*d);
					if (p == core->getData()->players.end() || p->second.tickCount < p->second.tickRate)
					{
						continue;
					}
					if (budgetExhausted)
					{
						deferredPlayers.push_back(*d);
						p->second.updatedThisTick = true;
						continue;
					}
					if (!p->second.delayedUpdate)
					{
						std::chrono::steady_clock::time_point updateStart = std::chrono::steady_clock::now();
						performPlayerUpdate(p->second, true);
						float elapsedTime = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - updateStart).count();
						spent += elapsedTime;
					}
					else
					{
						startManualUpdate(p->second, p->second.delayedUpdateType);
					}
					p->second.tickCount = 0;
					p->second.updatedThisTick = true;
					if (spent >= updateBudget)
					{
						budgetExhausted = true;
					}
				}
			}
			for (std::unordered_map<int, Player>::iterator p = core->getData()->players.begin(); p != core->getData()->players.end(); ++p)
			{
				if (p->second.updatedThisTick)
				{
					// Already handled by the deferred-first pass above. Do not
					// advance tickCount for this tick: the update reset it, so
					// advancing here would make the next one come a tick early.
					p->second.updatedThisTick = false;
					continue;
				}
				++p->second.tickCount;
				// Draining a pending chunk queue must not starve the regular
				// update, so the tick gate advances unconditionally and the
				// drain is only a fallback. See the chunk streamer for the
				// drain rate itself.
				if (p->second.tickCount >= p->second.tickRate)
				{
					if (budgetExhausted)
					{
						// Leave tickCount alone: the player stays due and is
						// retried first on the next tick.
						deferredPlayers.push_back(p->first);
						continue;
					}
					if (!p->second.delayedUpdate)
					{
						std::chrono::steady_clock::time_point updateStart = std::chrono::steady_clock::now();
						performPlayerUpdate(p->second, true);
						float elapsedTime = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - updateStart).count();
						spent += elapsedTime;
					}
					else
					{
						startManualUpdate(p->second, p->second.delayedUpdateType);
					}
					p->second.tickCount = 0;
					if (spent >= updateBudget)
					{
						budgetExhausted = true;
					}
				}
				else if (!budgetExhausted && core->getChunkStreamer()->getChunkStreamingEnabled() && p->second.processingChunks.any())
				{
					std::chrono::steady_clock::time_point drainStart = std::chrono::steady_clock::now();
					core->getChunkStreamer()->performPlayerChunkUpdate(p->second, true);
					spent += std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - drainStart).count();
					if (spent >= updateBudget)
					{
						budgetExhausted = true;
					}
				}
			}
		}
		if (++tickCount >= tickRate)
		{
			// Moving and attached items are global state, so they belong to
			// the global tick. When this pass was moved into the per-player
			// branch above, it ran on essentially every server tick as soon as
			// enough players were online (at most once per tick thanks to the
			// old guard, i.e. up to tickRate times more often than intended),
			// which is what made large servers fall over in 2.9.x. Update them
			// here, once per global tick, and keep the per-player gate for the
			// per-player streaming work only.
			processActiveItems();
			for (std::unordered_map<int, Player>::iterator p = core->getData()->players.begin(); p != core->getData()->players.end(); ++p)
			{
				std::vector<SharedCell> cells;
				core->getGrid()->findMinimalCellsForPlayer(p->second, cells);

				for (std::vector<int>::const_iterator t = core->getData()->typePriority.begin(); t != core->getData()->typePriority.end(); ++t)
				{
					switch (*t)
					{
						case STREAMER_TYPE_PICKUP:
						{
							if (!core->getData()->pickups.empty() && p->second.enabledItems[STREAMER_TYPE_PICKUP])
							{
								discoverPickups(p->second, cells);
							}
							break;
						}
						case STREAMER_TYPE_ACTOR:
						{
							if (!core->getData()->actors.empty() && p->second.enabledItems[STREAMER_TYPE_ACTOR])
							{
								discoverActors(p->second, cells);
							}
							break;
						}
					}
				}
			}

			for (std::vector<int>::const_iterator t = core->getData()->typePriority.begin(); t != core->getData()->typePriority.end(); ++t)
			{
				switch (*t)
				{
					case STREAMER_TYPE_PICKUP:
					{
						streamPickups();
						break;
					}
					case STREAMER_TYPE_ACTOR:
					{
						Utility::processPendingDestroyedActors();
						streamActors();
						break;
					}
				}
			}
			executeCallbacks();
			tickCount = 0;
		}
		calculateAverageElapsedTime();
		lastUpdateTime = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - currentTime).count();
	}
}

void Streamer::startManualUpdate(Player &player, int type)
{
	std::bitset<STREAMER_MAX_TYPES> enabledItems = player.enabledItems;
	if (player.delayedUpdate)
	{
		if (player.delayedUpdateTime.time_since_epoch() <= std::chrono::steady_clock::now().time_since_epoch())
		{
			if (player.delayedUpdateFreeze)
			{
				sampgdk::TogglePlayerControllable(player.playerId, true);
			}
			player.delayedUpdate = false;
		}
	}
	if (type >= 0 && type < STREAMER_MAX_TYPES)
	{
		if (core->getChunkStreamer()->getChunkStreamingEnabled())
		{
			switch (type)
			{
				case STREAMER_TYPE_OBJECT:
				{
					player.discoveredObjects.clear();
					player.existingObjects.clear();
					player.processingChunks.reset(STREAMER_TYPE_OBJECT);
					break;
				}
				case STREAMER_TYPE_MAP_ICON:
				{
					player.discoveredMapIcons.clear();
					player.existingMapIcons.clear();
					player.processingChunks.reset(STREAMER_TYPE_MAP_ICON);
					break;
				}
				case STREAMER_TYPE_3D_TEXT_LABEL:
				{
					player.discoveredTextLabels.clear();
					player.existingTextLabels.clear();
					player.processingChunks.reset(STREAMER_TYPE_3D_TEXT_LABEL);
					break;
				}
			}
		}
		player.enabledItems.reset();
		player.enabledItems.set(type);
	}
	else if (core->getChunkStreamer()->getChunkStreamingEnabled())
	{
		player.discoveredMapIcons.clear();
		player.discoveredObjects.clear();
		player.discoveredTextLabels.clear();
		player.existingMapIcons.clear();
		player.existingObjects.clear();
		player.existingTextLabels.clear();
		player.processingChunks.reset();
	}
	processActiveItems();
	performPlayerUpdate(player, false);
	if (core->getChunkStreamer()->getChunkStreamingEnabled())
	{
		core->getChunkStreamer()->performPlayerChunkUpdate(player, false);
	}
	player.enabledItems = enabledItems;
}

void Streamer::performPlayerUpdate(Player &player, bool automatic)
{
	Eigen::Vector3f delta = Eigen::Vector3f::Zero(), position = player.position;
	bool update = true;
	if (automatic)
	{
		player.interiorId = sampgdk::GetPlayerInterior(player.playerId);
		player.worldId = sampgdk::GetPlayerVirtualWorld(player.playerId);
		if (!player.updateUsingCameraPosition)
		{
			int state = sampgdk::GetPlayerState(player.playerId);
			if ((state != PLAYER_STATE_NONE && state != PLAYER_STATE_WASTED) || (state == PLAYER_STATE_SPECTATING && !player.requestingClass))
			{
				if (!sampgdk::IsPlayerInAnyVehicle(player.playerId))
				{
					sampgdk::GetPlayerPos(player.playerId, &player.position[0], &player.position[1], &player.position[2]);
				}
				else
				{
					sampgdk::GetVehiclePos(sampgdk::GetPlayerVehicleID(player.playerId), &player.position[0], &player.position[1], &player.position[2]);
				}
				if (player.position != position)
				{
					position = player.position;
					Eigen::Vector3f velocity = Eigen::Vector3f::Zero();
					if (state == PLAYER_STATE_ONFOOT)
					{
						sampgdk::GetPlayerVelocity(player.playerId, &velocity[0], &velocity[1], &velocity[2]);
					}
					else if (state == PLAYER_STATE_DRIVER || state == PLAYER_STATE_PASSENGER)
					{
						sampgdk::GetVehicleVelocity(sampgdk::GetPlayerVehicleID(player.playerId), &velocity[0], &velocity[1], &velocity[2]);
					}
					float velocityNorm = velocity.squaredNorm();
					if (velocityNorm > std::get<0>(velocityBoundaries) && velocityNorm < std::get<1>(velocityBoundaries))
					{
						delta = velocity * averageElapsedTime;
					}
				}
				else
				{
					update = player.updateWhenIdle;
				}
			}
			else
			{
				update = false;
			}
		}
		else
		{
			sampgdk::GetPlayerCameraPos(player.playerId, &player.position[0], &player.position[1], &player.position[2]);
		}
		if (player.delayedCheckpoint)
		{
			std::unordered_map<int, Item::SharedCheckpoint>::iterator c = core->getData()->checkpoints.find(player.delayedCheckpoint);
			if (c != core->getData()->checkpoints.end())
			{
				sampgdk::SetPlayerCheckpoint(player.playerId, c->second->position[0], c->second->position[1], c->second->position[2], c->second->size);
				if (c->second->streamCallbacks)
				{
					streamInCallbacks.push_back(std::make_tuple(STREAMER_TYPE_CP, c->first, player.playerId));
				}
				player.visibleCheckpoint = c->first;
			}
			player.delayedCheckpoint = 0;
		}
		else if (player.delayedRaceCheckpoint)
		{
			std::unordered_map<int, Item::SharedRaceCheckpoint>::iterator r = core->getData()->raceCheckpoints.find(player.delayedRaceCheckpoint);
			if (r != core->getData()->raceCheckpoints.end())
			{
				sampgdk::SetPlayerRaceCheckpoint(player.playerId, r->second->type, r->second->position[0], r->second->position[1], r->second->position[2], r->second->next[0], r->second->next[1], r->second->next[2], r->second->size);
				if (r->second->streamCallbacks)
				{
					streamInCallbacks.push_back(std::make_tuple(STREAMER_TYPE_RACE_CP, r->first, player.playerId));
				}
				player.visibleRaceCheckpoint = r->first;
			}
			player.delayedRaceCheckpoint = 0;
		}
	}
	// Nothing below touches the grid when the player is idle and idle updates
	// are disabled: the cell lookup would be computed and then thrown away by
	// the type loop below, so skip it entirely. This is the common case for
	// players that are not moving.
	if (!update)
	{
		return;
	}
	std::vector<SharedCell> cells;
	core->getGrid()->findAllCellsForPlayer(player, cells);
	if (!cells.empty())
	{
		if (!delta.isZero())
		{
			player.position += delta;
		}
		for (std::vector<int>::const_iterator t = core->getData()->typePriority.begin(); t != core->getData()->typePriority.end(); ++t)
		{
			switch (*t)
			{
				case STREAMER_TYPE_OBJECT:
				{
					if (!core->getData()->objects.empty() && player.enabledItems[STREAMER_TYPE_OBJECT])
					{
						if (core->getChunkStreamer()->getChunkStreamingEnabled())
						{
							core->getChunkStreamer()->discoverObjects(player, cells);
						}
						else
						{
							processObjects(player, cells);
						}
					}
					break;
				}
				case STREAMER_TYPE_CP:
				{
					if (!core->getData()->checkpoints.empty() && player.enabledItems[STREAMER_TYPE_CP])
					{
						processCheckpoints(player, cells);
					}
					break;
				}
				case STREAMER_TYPE_RACE_CP:
				{
					if (!core->getData()->raceCheckpoints.empty() && player.enabledItems[STREAMER_TYPE_RACE_CP])
					{
						processRaceCheckpoints(player, cells);
					}
					break;
				}
				case STREAMER_TYPE_MAP_ICON:
				{
					if (!core->getData()->mapIcons.empty() && player.enabledItems[STREAMER_TYPE_MAP_ICON])
					{
						if (core->getChunkStreamer()->getChunkStreamingEnabled())
						{
							core->getChunkStreamer()->discoverMapIcons(player, cells);
						}
						else
						{
							processMapIcons(player, cells);
						}
					}
					break;
				}
				case STREAMER_TYPE_3D_TEXT_LABEL:
				{
					if (!core->getData()->textLabels.empty() && player.enabledItems[STREAMER_TYPE_3D_TEXT_LABEL])
					{
						if (core->getChunkStreamer()->getChunkStreamingEnabled())
						{
							core->getChunkStreamer()->discoverTextLabels(player, cells);
						}
						else
						{
							processTextLabels(player, cells);
						}
					}
					break;
				}
				case STREAMER_TYPE_AREA:
				{
					if (!core->getData()->areas.empty() && player.enabledItems[STREAMER_TYPE_AREA])
					{
						if (!delta.isZero())
						{
							player.position = position;
						}
						processAreas(player, cells);
						if (!delta.isZero())
						{
							player.position += delta;
						}
					}
					break;
				}
			}
		}
		if (!delta.isZero())
		{
			player.position = position;
		}
	}
}

void Streamer::executeCallbacks()
{
	if (!areaLeaveCallbacks.empty())
	{
		std::multimap<int, std::tuple<int, int> > callbacks;
		std::swap(areaLeaveCallbacks, callbacks);
		for (std::multimap<int, std::tuple<int, int> >::reverse_iterator c = callbacks.rbegin(); c != callbacks.rend(); ++c)
		{
			std::unordered_map<int, Item::SharedArea>::iterator a = core->getData()->areas.find(std::get<0>(c->second));
			if (a != core->getData()->areas.end())
			{
				for (std::set<AMX*>::iterator i = core->getData()->interfaces.begin(); i != core->getData()->interfaces.end(); ++i)
				{
					int amxIndex = 0;
					if (!amx_FindPublic(*i, "OnPlayerLeaveDynamicArea", &amxIndex))
					{
						amx_Push(*i, static_cast<cell>(std::get<0>(c->second)));
						amx_Push(*i, static_cast<cell>(std::get<1>(c->second)));
						amx_Exec(*i, NULL, amxIndex);
					}
				}
			}
		}
	}
	if (!areaEnterCallbacks.empty())
	{
		std::multimap<int, std::tuple<int, int> > callbacks;
		std::swap(areaEnterCallbacks, callbacks);
		for (std::multimap<int, std::tuple<int, int> >::reverse_iterator c = callbacks.rbegin(); c != callbacks.rend(); ++c)
		{
			std::unordered_map<int, Item::SharedArea>::iterator a = core->getData()->areas.find(std::get<0>(c->second));
			if (a != core->getData()->areas.end())
			{
				for (std::set<AMX*>::iterator i = core->getData()->interfaces.begin(); i != core->getData()->interfaces.end(); ++i)
				{
					int amxIndex = 0;
					if (!amx_FindPublic(*i, "OnPlayerEnterDynamicArea", &amxIndex))
					{
						amx_Push(*i, static_cast<cell>(std::get<0>(c->second)));
						amx_Push(*i, static_cast<cell>(std::get<1>(c->second)));
						amx_Exec(*i, NULL, amxIndex);
					}
				}
			}
		}
	}
	if (!objectMoveCallbacks.empty())
	{
		std::vector<int> callbacks;
		std::swap(objectMoveCallbacks, callbacks);
		for (std::vector<int>::const_iterator c = callbacks.begin(); c != callbacks.end(); ++c)
		{
			std::unordered_map<int, Item::SharedObject>::iterator o = core->getData()->objects.find(*c);
			if (o != core->getData()->objects.end())
			{
				for (std::set<AMX*>::iterator i = core->getData()->interfaces.begin(); i != core->getData()->interfaces.end(); ++i)
				{
					int amxIndex = 0;
					if (!amx_FindPublic(*i, "OnDynamicObjectMoved", &amxIndex))
					{
						amx_Push(*i, static_cast<cell>(*c));
						amx_Exec(*i, NULL, amxIndex);
					}
				}
			}
		}
	}
	if (!streamInCallbacks.empty())
	{
		std::vector<std::tuple<int, int, int> > callbacks;
		std::swap(streamInCallbacks, callbacks);
		for (std::vector<std::tuple<int, int, int> >::const_iterator c = callbacks.begin(); c != callbacks.end(); ++c)
		{
			switch (std::get<0>(*c))
			{
				case STREAMER_TYPE_OBJECT:
				{
					if (core->getData()->objects.find(std::get<1>(*c)) == core->getData()->objects.end())
					{
						continue;
					}
					break;
				}
				case STREAMER_TYPE_PICKUP:
				{
					if (core->getData()->pickups.find(std::get<1>(*c)) == core->getData()->pickups.end())
					{
						continue;
					}
					break;
				}
				case STREAMER_TYPE_CP:
				{
					if (core->getData()->checkpoints.find(std::get<1>(*c)) == core->getData()->checkpoints.end())
					{
						continue;
					}
					break;
				}
				case STREAMER_TYPE_RACE_CP:
				{
					if (core->getData()->raceCheckpoints.find(std::get<1>(*c)) == core->getData()->raceCheckpoints.end())
					{
						continue;
					}
					break;
				}
				case STREAMER_TYPE_MAP_ICON:
				{
					if (core->getData()->mapIcons.find(std::get<1>(*c)) == core->getData()->mapIcons.end())
					{
						continue;
					}
					break;
				}
				case STREAMER_TYPE_3D_TEXT_LABEL:
				{
					if (core->getData()->textLabels.find(std::get<1>(*c)) == core->getData()->textLabels.end())
					{
						continue;
					}
					break;
				}
			}
			for (std::set<AMX*>::iterator i = core->getData()->interfaces.begin(); i != core->getData()->interfaces.end(); ++i)
			{
				int amxIndex = 0;
				if (!amx_FindPublic(*i, "Streamer_OnItemStreamIn", &amxIndex))
				{
					amx_Push(*i, static_cast<cell>(std::get<2>(*c)));
					amx_Push(*i, static_cast<cell>(std::get<1>(*c)));
					amx_Push(*i, static_cast<cell>(std::get<0>(*c)));
					amx_Exec(*i, NULL, amxIndex);
				}
			}
		}
	}
	if (!streamOutCallbacks.empty())
	{
		std::vector<std::tuple<int, int, int> > callbacks;
		std::swap(streamOutCallbacks, callbacks);
		for (std::vector<std::tuple<int, int, int> >::const_iterator c = callbacks.begin(); c != callbacks.end(); ++c)
		{
			switch (std::get<0>(*c))
			{
				case STREAMER_TYPE_OBJECT:
				{
					if (core->getData()->objects.find(std::get<1>(*c)) == core->getData()->objects.end())
					{
						continue;
					}
					break;
				}
				case STREAMER_TYPE_PICKUP:
				{
					if (core->getData()->pickups.find(std::get<1>(*c)) == core->getData()->pickups.end())
					{
						continue;
					}
					break;
				}
				case STREAMER_TYPE_CP:
				{
					if (core->getData()->checkpoints.find(std::get<1>(*c)) == core->getData()->checkpoints.end())
					{
						continue;
					}
					break;
				}
				case STREAMER_TYPE_RACE_CP:
				{
					if (core->getData()->raceCheckpoints.find(std::get<1>(*c)) == core->getData()->raceCheckpoints.end())
					{
						continue;
					}
					break;
				}
				case STREAMER_TYPE_MAP_ICON:
				{
					if (core->getData()->mapIcons.find(std::get<1>(*c)) == core->getData()->mapIcons.end())
					{
						continue;
					}
					break;
				}
				case STREAMER_TYPE_3D_TEXT_LABEL:
				{
					if (core->getData()->textLabels.find(std::get<1>(*c)) == core->getData()->textLabels.end())
					{
						continue;
					}
					break;
				}
			}
			for (std::set<AMX*>::iterator i = core->getData()->interfaces.begin(); i != core->getData()->interfaces.end(); ++i)
			{
				int amxIndex = 0;
				if (!amx_FindPublic(*i, "Streamer_OnItemStreamOut", &amxIndex))
				{
					amx_Push(*i, static_cast<cell>(std::get<2>(*c)));
					amx_Push(*i, static_cast<cell>(std::get<1>(*c)));
					amx_Push(*i, static_cast<cell>(std::get<0>(*c)));
					amx_Exec(*i, NULL, amxIndex);
				}
			}
		}
	}
}

void Streamer::discoverActors(Player &player, const std::vector<SharedCell> &cells)
{
	if (!sampgdk::IsPlayerNPC(player.playerId))
	{
		for (std::vector<SharedCell>::const_iterator c = cells.begin(); c != cells.end(); ++c)
		{
			for (std::unordered_map<int, Item::SharedActor>::const_iterator a = (*c)->actors.begin(); a != (*c)->actors.end(); ++a)
			{
				for (std::unordered_set<int>::const_iterator w = a->second->worlds.begin(); w != a->second->worlds.end(); ++w)
				{
					if (player.worldId != *w && *w != -1)
					{
						continue;
					}

					std::unordered_map<std::pair<int, int>, Item::SharedActor, pair_hash>::iterator d = core->getData()->discoveredActors.find(std::make_pair(a->first, *w));
					if (d == core->getData()->discoveredActors.end())
					{
						const int playerWorldId = *w == -1 ? -1 : player.worldId;
						if (doesPlayerSatisfyConditions(a->second->players, player.playerId, a->second->interiors, player.interiorId, a->second->worlds, playerWorldId, a->second->areas, player.internalAreas, a->second->inverseAreaChecking))
						{
							if (a->second->comparableStreamDistance < STREAMER_STATIC_DISTANCE_CUTOFF || Utility::isWithinComparableDistance(player.position, Eigen::Vector3f(a->second->position + a->second->positionOffset), a->second->comparableStreamDistance * player.radiusMultipliers[STREAMER_TYPE_ACTOR]))
							{
								core->getData()->discoveredActors.insert(std::make_pair(std::make_pair(a->first, *w), a->second));
							}
						}
					}
				}
			}
		}
	}
}

void Streamer::streamActors()
{
	std::unordered_map<std::pair<int, int>, int, pair_hash>::iterator i = core->getData()->internalActors.begin();
	while (i != core->getData()->internalActors.end())
	{
		std::unordered_map<std::pair<int, int>, Item::SharedActor, pair_hash>::iterator d = core->getData()->discoveredActors.find(i->first);
		if (d == core->getData()->discoveredActors.end())
		{
			sampgdk::DestroyActor(i->second);
			i = core->getData()->internalActors.erase(i);
		}
		else
		{
			core->getData()->discoveredActors.erase(d);
			++i;
		}
	}
	std::multimap<int, std::pair<int, Item::SharedActor> > sortedActors;
	for (std::unordered_map<std::pair<int, int>, Item::SharedActor, pair_hash>::iterator d = core->getData()->discoveredActors.begin(); d != core->getData()->discoveredActors.end(); ++d)
	{
		sortedActors.insert(std::make_pair(d->second->priority, std::make_pair(d->first.second, d->second)));
	}
	core->getData()->discoveredActors.clear();
	for (std::multimap<int, std::pair<int, Item::SharedActor> >::iterator s = sortedActors.begin(); s != sortedActors.end(); ++s)
	{
		if (core->getData()->internalActors.size() == core->getData()->getGlobalMaxVisibleItems(STREAMER_TYPE_ACTOR))
		{
			break;
		}
		int internalId = sampgdk::CreateActor(s->second.second->modelId, s->second.second->position[0], s->second.second->position[1], s->second.second->position[2], s->second.second->rotation);
		if (internalId == INVALID_ACTOR_ID)
		{
			break;
		}
		sampgdk::SetActorInvulnerable(internalId, s->second.second->invulnerable);
		sampgdk::SetActorHealth(internalId, s->second.second->health);
		sampgdk::SetActorVirtualWorld(internalId, s->second.first);
		if (s->second.second->anim)
		{
			sampgdk::ApplyActorAnimation(internalId, s->second.second->anim->lib.c_str(), s->second.second->anim->name.c_str(), s->second.second->anim->delta, s->second.second->anim->loop, s->second.second->anim->lockx, s->second.second->anim->locky, s->second.second->anim->freeze, s->second.second->anim->time);
		}
		core->getData()->internalActors.insert(std::make_pair(std::make_pair(s->second.second->actorId, s->second.first), internalId));
	}
}

void Streamer::processAreas(Player &player, const std::vector<SharedCell> &cells)
{
	int state = sampgdk::GetPlayerState(player.playerId);
	for (std::vector<SharedCell>::const_iterator c = cells.begin(); c != cells.end(); ++c)
	{
		for (std::unordered_map<int, Item::SharedArea>::const_iterator a = (*c)->areas.begin(); a != (*c)->areas.end(); ++a)
		{
			Streamer::processPlayerArea(player, a->second, state);
		}
	}
}

bool Streamer::processPlayerArea(Player &player, const Item::SharedArea &a, const int state)
{
	bool inArea = false;
	if (doesPlayerSatisfyConditions(a->players, player.playerId, a->interiors, player.interiorId, a->worlds, player.worldId) && ((!a->spectateMode && state != PLAYER_STATE_SPECTATING) || a->spectateMode))
	{
		inArea = Utility::isPointInArea(player.position, a);
	}
	std::unordered_set<int>::iterator foundArea = player.internalAreas.find(a->areaId);
	if (inArea)
	{
		if (foundArea == player.internalAreas.end())
		{
			player.internalAreas.insert(a->areaId);
			areaEnterCallbacks.insert(std::make_pair(a->priority, std::make_tuple(a->areaId, player.playerId)));
		}
		if (a->cell)
		{
			player.visibleCell->areas.insert(std::make_pair(a->areaId, a));
		}
	}
	else
	{
		if (foundArea != player.internalAreas.end())
		{
			player.internalAreas.erase(foundArea);
			areaLeaveCallbacks.insert(std::make_pair(a->priority, std::make_tuple(a->areaId, player.playerId)));
		}
	}
	return inArea;
}

void Streamer::processCheckpoints(Player &player, const std::vector<SharedCell> &cells)
{
	std::vector<Item::Candidate<Item::SharedCheckpoint> > &discoveredCheckpoints = player.discoveredCheckpointCandidates;
	discoveredCheckpoints.clear();
	for (std::vector<SharedCell>::const_iterator c = cells.begin(); c != cells.end(); ++c)
	{
		for (std::unordered_map<int, Item::SharedCheckpoint>::const_iterator d = (*c)->checkpoints.begin(); d != (*c)->checkpoints.end(); ++d)
		{
			float distance = std::numeric_limits<float>::infinity();
			if (doesPlayerSatisfyConditions(d->second->players, player.playerId, d->second->interiors, player.interiorId, d->second->worlds, player.worldId, d->second->areas, player.internalAreas, d->second->inverseAreaChecking))
			{
				if (d->second->comparableStreamDistance < STREAMER_STATIC_DISTANCE_CUTOFF)
				{
					distance = std::numeric_limits<float>::infinity() * -1.0f;
				}
				else
				{
					distance = Utility::comparableDistance(player.position, Eigen::Vector3f(d->second->position + d->second->positionOffset), d->second->comparableStreamDistance * player.radiusMultipliers[STREAMER_TYPE_CP]);
				}
			}
			if (distance < (d->second->comparableStreamDistance * player.radiusMultipliers[STREAMER_TYPE_CP]))
			{
				discoveredCheckpoints.push_back(Item::Candidate<Item::SharedCheckpoint>(d->second->priority, distance, d->second));
			}
			else
			{
				if (d->first == player.visibleCheckpoint)
				{
					sampgdk::DisablePlayerCheckpoint(player.playerId);
					if (d->second->streamCallbacks)
					{
						streamOutCallbacks.push_back(std::make_tuple(STREAMER_TYPE_CP, d->second->checkpointId, player.playerId));
					}
					player.activeCheckpoint = 0;
					player.visibleCheckpoint = 0;

				}
			}
		}
	}
	if (!discoveredCheckpoints.empty())
	{
		// Only the best candidate is ever used, so a min_element under the same
		// comparator replaces the multimap's begin() without a full sort.
		const Item::SharedCheckpoint &checkpoint = std::min_element(discoveredCheckpoints.begin(), discoveredCheckpoints.end(), Item::CandidateCompare<Item::SharedCheckpoint>())->item;
		if (checkpoint->checkpointId != player.visibleCheckpoint)
		{
			if (player.visibleCheckpoint)
			{
				sampgdk::DisablePlayerCheckpoint(player.playerId);
				if (checkpoint->streamCallbacks)
				{
					streamOutCallbacks.push_back(std::make_tuple(STREAMER_TYPE_CP, checkpoint->checkpointId, player.playerId));
				}
				player.activeCheckpoint = 0;
			}
			player.delayedCheckpoint = checkpoint->checkpointId;
		}
		if (checkpoint->cell)
		{
			player.visibleCell->checkpoints.insert(std::make_pair(checkpoint->checkpointId, checkpoint));
		}
	}
}

void Streamer::processMapIcons(Player &player, const std::vector<SharedCell> &cells)
{
	std::vector<Item::Candidate<Item::SharedMapIcon> > &discoveredMapIcons = player.discoveredMapIconCandidates;
	std::vector<Item::Candidate<Item::SharedMapIcon> > &existingMapIcons = player.existingMapIconCandidates;
	discoveredMapIcons.clear();
	existingMapIcons.clear();
	for (std::vector<SharedCell>::const_iterator c = cells.begin(); c != cells.end(); ++c)
	{
		for (std::unordered_map<int, Item::SharedMapIcon>::const_iterator m = (*c)->mapIcons.begin(); m != (*c)->mapIcons.end(); ++m)
		{
			float distance = std::numeric_limits<float>::infinity();
			if (doesPlayerSatisfyConditions(m->second->players, player.playerId, m->second->interiors, player.interiorId, m->second->worlds, player.worldId, m->second->areas, player.internalAreas, m->second->inverseAreaChecking))
			{
				if (m->second->comparableStreamDistance < STREAMER_STATIC_DISTANCE_CUTOFF)
				{
					distance = std::numeric_limits<float>::infinity() * -1.0f;
				}
				else
				{
					distance = Utility::comparableDistance(player.position, Eigen::Vector3f(m->second->position + m->second->positionOffset), m->second->comparableStreamDistance * player.radiusMultipliers[STREAMER_TYPE_MAP_ICON]);
				}
			}
			const bool streamed = player.internalMapIcons.contains(m->first);
			if (distance < (m->second->comparableStreamDistance * player.radiusMultipliers[STREAMER_TYPE_MAP_ICON]))
			{
				if (!streamed)
				{
					discoveredMapIcons.push_back(Item::Candidate<Item::SharedMapIcon>(m->second->priority, distance, m->second));
				}
				else
				{
					if (m->second->cell)
					{
						player.visibleCell->mapIcons.insert(*m);
					}
					existingMapIcons.push_back(Item::Candidate<Item::SharedMapIcon>(m->second->priority, distance, m->second));
				}
			}
			else if (streamed)
			{
				std::unordered_map<int, int>::iterator i = player.internalMapIcons.find(m->first);
				if (i != player.internalMapIcons.end())
				{
					sampgdk::RemovePlayerMapIcon(player.playerId, i->second);
					if (m->second->streamCallbacks)
					{
						streamOutCallbacks.push_back(std::make_tuple(STREAMER_TYPE_MAP_ICON, m->first, player.playerId));
					}
					player.mapIconIdentifier.remove(i->second, player.internalMapIcons.size());
					player.internalMapIcons.erase(i);
				}
			}
		}
	}
	std::sort(discoveredMapIcons.begin(), discoveredMapIcons.end(), Item::CandidateCompare<Item::SharedMapIcon>());
	std::sort(existingMapIcons.begin(), existingMapIcons.end(), Item::CandidateCompare<Item::SharedMapIcon>());
	for (std::vector<Item::Candidate<Item::SharedMapIcon> >::iterator d = discoveredMapIcons.begin(); d != discoveredMapIcons.end(); ++d)
	{
		const Item::SharedMapIcon &mapIcon = d->item;
		std::unordered_map<int, int>::iterator i = player.internalMapIcons.find(mapIcon->mapIconId);
		if (i != player.internalMapIcons.end())
		{
			continue;
		}
		if (player.internalMapIcons.size() == player.maxVisibleMapIcons)
		{
			// existingMapIcons is sorted ascending under the multimap's own
			// comparator, so back() is exactly the old rbegin() element.
			if (!existingMapIcons.empty())
			{
				const Item::Candidate<Item::SharedMapIcon> &e = existingMapIcons.back();
				if (e.priority < d->priority || (e.distance > STREAMER_STATIC_DISTANCE_CUTOFF && d->distance < e.distance))
				{
					std::unordered_map<int, int>::iterator j = player.internalMapIcons.find(e.item->mapIconId);
					if (j != player.internalMapIcons.end())
					{
						sampgdk::RemovePlayerMapIcon(player.playerId, j->second);
						if (e.item->streamCallbacks)
						{
							streamOutCallbacks.push_back(std::make_tuple(STREAMER_TYPE_MAP_ICON, e.item->mapIconId, player.playerId));
						}
						player.mapIconIdentifier.remove(j->second, player.internalMapIcons.size());
						player.internalMapIcons.erase(j);
					}
					if (e.item->cell)
					{
						player.visibleCell->mapIcons.erase(e.item->mapIconId);
					}
					existingMapIcons.pop_back();
				}
			}
			if (player.internalMapIcons.size() == player.maxVisibleMapIcons)
			{
				break;
			}
		}
		int internalId = player.mapIconIdentifier.get();
		sampgdk::SetPlayerMapIcon(player.playerId, internalId, mapIcon->position[0], mapIcon->position[1], mapIcon->position[2], mapIcon->type, mapIcon->color, mapIcon->style);
		if (mapIcon->streamCallbacks)
		{
			streamInCallbacks.push_back(std::make_tuple(STREAMER_TYPE_MAP_ICON, mapIcon->mapIconId, player.playerId));
		}
		player.internalMapIcons.insert(std::make_pair(mapIcon->mapIconId, internalId));
		if (mapIcon->cell)
		{
			player.visibleCell->mapIcons.insert(std::make_pair(mapIcon->mapIconId, mapIcon));
		}
	}
}

void Streamer::processObjects(Player &player, const std::vector<SharedCell> &cells)
{
	// Reused per-player buffers. The ordering is identical to the multimap this
	// replaces, but there is no red-black tree node allocation per candidate.
	std::vector<Item::Candidate<Item::SharedObject> > &discoveredObjects = player.discoveredObjectCandidates;
	std::vector<Item::Candidate<Item::SharedObject> > &existingObjects = player.existingObjectCandidates;
	discoveredObjects.clear();
	existingObjects.clear();
	for (std::vector<SharedCell>::const_iterator c = cells.begin(); c != cells.end(); ++c)
	{
		for (std::unordered_map<int, Item::SharedObject>::const_iterator o = (*c)->objects.begin(); o != (*c)->objects.end(); ++o)
		{
			float distance = std::numeric_limits<float>::infinity();
			if (doesPlayerSatisfyConditions(o->second->players, player.playerId, o->second->interiors, player.interiorId, o->second->attach ? o->second->attach->worlds : o->second->worlds, player.worldId, o->second->areas, player.internalAreas, o->second->inverseAreaChecking))
			{
				if (o->second->comparableStreamDistance < STREAMER_STATIC_DISTANCE_CUTOFF)
				{
					distance = std::numeric_limits<float>::infinity() * -1.0f;
				}
				else
				{
					if (o->second->attach)
					{
						distance = Utility::comparableDistance(player.position, o->second->attach->position, o->second->comparableStreamDistance * player.radiusMultipliers[STREAMER_TYPE_OBJECT]) + std::numeric_limits<float>::epsilon();
					}
					else
					{
						distance = Utility::comparableDistance(player.position, Eigen::Vector3f(o->second->position + o->second->positionOffset), o->second->comparableStreamDistance * player.radiusMultipliers[STREAMER_TYPE_OBJECT]);
					}
				}
			}
			const bool streamed = player.internalObjects.contains(o->first);
			if (distance < (o->second->comparableStreamDistance * player.radiusMultipliers[STREAMER_TYPE_OBJECT]))
			{
				if (!streamed)
				{
					discoveredObjects.push_back(Item::Candidate<Item::SharedObject>(o->second->priority, distance, o->second));
				}
				else
				{
					if (o->second->cell)
					{
						player.visibleCell->objects.insert(*o);
					}
					existingObjects.push_back(Item::Candidate<Item::SharedObject>(o->second->priority, distance, o->second));
				}
			}
			else if (streamed)
			{
				std::unordered_map<int, int>::iterator i = player.internalObjects.find(o->first);
				if (i != player.internalObjects.end())
				{
					sampgdk::DestroyPlayerObject(player.playerId, i->second);
					if (o->second->streamCallbacks)
					{
						streamOutCallbacks.push_back(std::make_tuple(STREAMER_TYPE_OBJECT, o->first, player.playerId));
					}
					player.internalObjects.erase(i);
				}
			}
		}
	}
	// Sort ascending under the same comparator the multimap used, so the loop
	// below visits candidates in exactly the order it always did.
	std::sort(discoveredObjects.begin(), discoveredObjects.end(), Item::CandidateCompare<Item::SharedObject>());
	std::sort(existingObjects.begin(), existingObjects.end(), Item::CandidateCompare<Item::SharedObject>());
	for (std::vector<Item::Candidate<Item::SharedObject> >::iterator d = discoveredObjects.begin(); d != discoveredObjects.end(); ++d)
	{
		const Item::SharedObject &object = d->item;
		std::unordered_map<int, int>::iterator i = player.internalObjects.find(object->objectId);
		if (i != player.internalObjects.end())
		{
			continue;
		}
		int internalBaseId = INVALID_STREAMER_ID;
		if (object->attach)
		{
			if (object->attach->object != INVALID_STREAMER_ID)
			{
				std::unordered_map<int, int>::iterator j = player.internalObjects.find(object->attach->object);
				if (j == player.internalObjects.end())
				{
					continue;
				}
				internalBaseId = j->second;
			}
		}
		if (player.internalObjects.size() == player.currentVisibleObjects)
		{
			// existingObjects is sorted ascending under that same comparator,
			// so back() is exactly the element the old rbegin() pointed at.
			if (!existingObjects.empty())
			{
				const Item::Candidate<Item::SharedObject> &e = existingObjects.back();
				if (e.priority < d->priority || (e.distance > STREAMER_STATIC_DISTANCE_CUTOFF && d->distance < e.distance))
				{
					std::unordered_map<int, int>::iterator j = player.internalObjects.find(e.item->objectId);
					if (j != player.internalObjects.end())
					{
						sampgdk::DestroyPlayerObject(player.playerId, j->second);
						if (e.item->streamCallbacks)
						{
							streamOutCallbacks.push_back(std::make_tuple(STREAMER_TYPE_OBJECT, e.item->objectId, player.playerId));
						}
						player.internalObjects.erase(j);
					}
					if (e.item->cell)
					{
						player.visibleCell->objects.erase(e.item->objectId);
					}
					existingObjects.pop_back();
				}
			}
		}
		if (player.internalObjects.size() == player.maxVisibleObjects)
		{
			player.currentVisibleObjects = player.internalObjects.size();
			break;
		}
		int internalId = sampgdk::CreatePlayerObject(player.playerId, object->modelId, object->position[0], object->position[1], object->position[2], object->rotation[0], object->rotation[1], object->rotation[2], object->drawDistance);
		if (internalId == INVALID_OBJECT_ID)
		{
			player.currentVisibleObjects = player.internalObjects.size();
			break;
		}
		if (object->streamCallbacks)
		{
			streamInCallbacks.push_back(std::make_tuple(STREAMER_TYPE_OBJECT, object->objectId, player.playerId));
		}
		if (object->attach)
		{
			if (internalBaseId != INVALID_STREAMER_ID)
			{
				static AMX_NATIVE native = sampgdk::FindNative("AttachPlayerObjectToObject");
				if (native != NULL)
				{
					sampgdk::InvokeNative(native, "dddffffffb", player.playerId, internalId, internalBaseId, object->attach->positionOffset[0], object->attach->positionOffset[1], object->attach->positionOffset[2], object->attach->rotation[0], object->attach->rotation[1], object->attach->rotation[2], object->attach->syncRotation);
				}
			}
			else if (object->attach->player != INVALID_PLAYER_ID)
			{
				static AMX_NATIVE native = sampgdk::FindNative("AttachPlayerObjectToPlayer");
				if (native != NULL)
				{
					sampgdk::InvokeNative(native, "dddffffffd", player.playerId, internalId, object->attach->player, object->attach->positionOffset[0], object->attach->positionOffset[1], object->attach->positionOffset[2], object->attach->rotation[0], object->attach->rotation[1], object->attach->rotation[2], 1);
				}
			}
			else if (object->attach->vehicle != INVALID_VEHICLE_ID)
			{
				sampgdk::AttachPlayerObjectToVehicle(player.playerId, internalId, object->attach->vehicle, object->attach->positionOffset[0], object->attach->positionOffset[1], object->attach->positionOffset[2], object->attach->rotation[0], object->attach->rotation[1], object->attach->rotation[2]);
			}
		}
		else if (object->move)
		{
			sampgdk::MovePlayerObject(player.playerId, internalId, std::get<0>(object->move->position)[0], std::get<0>(object->move->position)[1], std::get<0>(object->move->position)[2], object->move->speed, std::get<0>(object->move->rotation)[0], std::get<0>(object->move->rotation)[1], std::get<0>(object->move->rotation)[2]);
		}
		for (std::unordered_map<int, Item::Object::Material>::iterator m = object->materials.begin(); m != object->materials.end(); ++m)
		{
			if (m->second.main)
			{
				sampgdk::SetPlayerObjectMaterial(player.playerId, internalId, m->first, m->second.main->modelId, m->second.main->txdFileName.c_str(), m->second.main->textureName.c_str(), m->second.main->materialColor);
			}
			else if (m->second.text)
			{
				sampgdk::SetPlayerObjectMaterialText(player.playerId, internalId, m->second.text->materialText.c_str(), m->first, m->second.text->materialSize, m->second.text->fontFace.c_str(), m->second.text->fontSize, m->second.text->bold, m->second.text->fontColor, m->second.text->backColor, m->second.text->textAlignment);
			}
		}
		if (object->noCameraCollision)
		{
			sampgdk::SetPlayerObjectNoCameraCol(player.playerId, internalId);
		}
		player.internalObjects.insert(std::make_pair(object->objectId, internalId));
		if (object->cell)
		{
			player.visibleCell->objects.insert(std::make_pair(object->objectId, object));
		}
	}
}

void Streamer::discoverPickups(Player &player, const std::vector<SharedCell> &cells)
{
	for (std::vector<SharedCell>::const_iterator c = cells.begin(); c != cells.end(); ++c)
	{
		for (std::unordered_map<int, Item::SharedPickup>::const_iterator p = (*c)->pickups.begin(); p != (*c)->pickups.end(); ++p)
		{
			for (std::unordered_set<int>::const_iterator w = p->second->worlds.begin(); w != p->second->worlds.end(); ++w)
			{
				if (player.worldId != *w && *w != -1)
				{
					continue;
				}

				std::unordered_map<std::pair<int, int>, Item::SharedPickup, pair_hash>::iterator d = core->getData()->discoveredPickups.find(std::make_pair(p->first, *w));
				if (d == core->getData()->discoveredPickups.end())
				{
					const int playerWorldId = *w == -1 ? -1 : player.worldId;
					if (doesPlayerSatisfyConditions(p->second->players, player.playerId, p->second->interiors, player.interiorId, p->second->worlds, playerWorldId, p->second->areas, player.internalAreas, p->second->inverseAreaChecking))
					{
						if (p->second->comparableStreamDistance < STREAMER_STATIC_DISTANCE_CUTOFF || Utility::isWithinComparableDistance(player.position, Eigen::Vector3f(p->second->position + p->second->positionOffset), p->second->comparableStreamDistance * player.radiusMultipliers[STREAMER_TYPE_PICKUP]))
						{
							core->getData()->discoveredPickups.insert(std::make_pair(std::make_pair(p->first, *w), p->second));
						}
					}
				}
			}
		}
	}
}

void Streamer::streamPickups()
{
	std::unordered_map<std::pair<int, int>, int, pair_hash>::iterator i = core->getData()->internalPickups.begin();
	while (i != core->getData()->internalPickups.end())
	{
		std::unordered_map<std::pair<int, int>, Item::SharedPickup, pair_hash>::iterator d = core->getData()->discoveredPickups.find(i->first);
		if (d == core->getData()->discoveredPickups.end())
		{
			sampgdk::DestroyPickup(i->second);
			std::unordered_map<int, Item::SharedPickup>::iterator p = core->getData()->pickups.find(i->first.first);
			if (p != core->getData()->pickups.end())
			{
				if (p->second->streamCallbacks)
				{
					streamOutCallbacks.push_back(std::make_tuple(STREAMER_TYPE_PICKUP, i->first.first, INVALID_PLAYER_ID));
				}
			}
			i = core->getData()->internalPickups.erase(i);
		}
		else
		{
			core->getData()->discoveredPickups.erase(d);
			++i;
		}
	}
	std::multimap<int, std::pair<int, Item::SharedPickup> > sortedPickups;
	for (std::unordered_map<std::pair<int, int>, Item::SharedPickup, pair_hash>::iterator d = core->getData()->discoveredPickups.begin(); d != core->getData()->discoveredPickups.end(); ++d)
	{
		sortedPickups.insert(std::make_pair(d->second->priority, std::make_pair(d->first.second, d->second)));
	}
	core->getData()->discoveredPickups.clear();
	for (std::multimap<int, std::pair<int, Item::SharedPickup> >::iterator s = sortedPickups.begin(); s != sortedPickups.end(); ++s)
	{
		if (core->getData()->internalPickups.size() == core->getData()->getGlobalMaxVisibleItems(STREAMER_TYPE_PICKUP))
		{
			break;
		}
		int internalId = sampgdk::CreatePickup(s->second.second->modelId, s->second.second->type, s->second.second->position[0], s->second.second->position[1], s->second.second->position[2], s->second.first);
		if (internalId == INVALID_PICKUP_ID)
		{
			break;
		}
		if (s->second.second->streamCallbacks)
		{
			streamInCallbacks.push_back(std::make_tuple(STREAMER_TYPE_PICKUP, s->second.second->pickupId, INVALID_PLAYER_ID));
		}
		core->getData()->internalPickups.insert(std::make_pair(std::make_pair(s->second.second->pickupId, s->second.first), internalId));
	}
}

void Streamer::processRaceCheckpoints(Player &player, const std::vector<SharedCell> &cells)
{
	std::vector<Item::Candidate<Item::SharedRaceCheckpoint> > &discoveredRaceCheckpoints = player.discoveredRaceCheckpointCandidates;
	discoveredRaceCheckpoints.clear();
	for (std::vector<SharedCell>::const_iterator c = cells.begin(); c != cells.end(); ++c)
	{
		for (std::unordered_map<int, Item::SharedRaceCheckpoint>::const_iterator r = (*c)->raceCheckpoints.begin(); r != (*c)->raceCheckpoints.end(); ++r)
		{
			float distance = std::numeric_limits<float>::infinity();
			if (doesPlayerSatisfyConditions(r->second->players, player.playerId, r->second->interiors, player.interiorId, r->second->worlds, player.worldId, r->second->areas, player.internalAreas, r->second->inverseAreaChecking))
			{
				if (r->second->comparableStreamDistance < STREAMER_STATIC_DISTANCE_CUTOFF)
				{
					distance = std::numeric_limits<float>::infinity() * -1.0f;
				}
				else
				{
					distance = Utility::comparableDistance(player.position, Eigen::Vector3f(r->second->position + r->second->positionOffset), r->second->comparableStreamDistance * player.radiusMultipliers[STREAMER_TYPE_RACE_CP]);
				}
			}
			if (distance < (r->second->comparableStreamDistance * player.radiusMultipliers[STREAMER_TYPE_RACE_CP]))
			{
				discoveredRaceCheckpoints.push_back(Item::Candidate<Item::SharedRaceCheckpoint>(r->second->priority, distance, r->second));
			}
			else
			{
				if (r->first == player.visibleRaceCheckpoint)
				{
					sampgdk::DisablePlayerRaceCheckpoint(player.playerId);
					if (r->second->streamCallbacks)
					{
						streamOutCallbacks.push_back(std::make_tuple(STREAMER_TYPE_RACE_CP, r->second->raceCheckpointId, player.playerId));
					}
					player.activeRaceCheckpoint = 0;
					player.visibleRaceCheckpoint = 0;
				}
			}
		}
	}
	if (!discoveredRaceCheckpoints.empty())
	{
		// Only the best candidate is ever used, so a min_element under the same
		// comparator replaces the multimap's begin() without a full sort.
		const Item::SharedRaceCheckpoint &raceCheckpoint = std::min_element(discoveredRaceCheckpoints.begin(), discoveredRaceCheckpoints.end(), Item::CandidateCompare<Item::SharedRaceCheckpoint>())->item;
		if (raceCheckpoint->raceCheckpointId != player.visibleRaceCheckpoint)
		{
			if (player.visibleRaceCheckpoint)
			{
				sampgdk::DisablePlayerRaceCheckpoint(player.playerId);
				if (raceCheckpoint->streamCallbacks)
				{
					streamOutCallbacks.push_back(std::make_tuple(STREAMER_TYPE_RACE_CP, raceCheckpoint->raceCheckpointId, player.playerId));
				}
				player.activeRaceCheckpoint = 0;
			}
			player.delayedRaceCheckpoint = raceCheckpoint->raceCheckpointId;
		}
		if (raceCheckpoint->cell)
		{
			player.visibleCell->raceCheckpoints.insert(std::make_pair(raceCheckpoint->raceCheckpointId, raceCheckpoint));
		}
	}
}

void Streamer::processTextLabels(Player &player, const std::vector<SharedCell> &cells)
{
	std::vector<Item::Candidate<Item::SharedTextLabel> > &discoveredTextLabels = player.discoveredTextLabelCandidates;
	std::vector<Item::Candidate<Item::SharedTextLabel> > &existingTextLabels = player.existingTextLabelCandidates;
	discoveredTextLabels.clear();
	existingTextLabels.clear();
	for (std::vector<SharedCell>::const_iterator c = cells.begin(); c != cells.end(); ++c)
	{
		for (std::unordered_map<int, Item::SharedTextLabel>::const_iterator t = (*c)->textLabels.begin(); t != (*c)->textLabels.end(); ++t)
		{
			float distance = std::numeric_limits<float>::infinity();
			if (doesPlayerSatisfyConditions(t->second->players, player.playerId, t->second->interiors, player.interiorId, t->second->attach ? t->second->attach->worlds : t->second->worlds, player.worldId, t->second->areas, player.internalAreas, t->second->inverseAreaChecking))
			{
				if (t->second->comparableStreamDistance < STREAMER_STATIC_DISTANCE_CUTOFF)
				{
					distance = std::numeric_limits<float>::infinity() * -1.0f;
				}
				else
				{
					if (t->second->attach)
					{
						distance = Utility::comparableDistance(player.position, t->second->attach->position, t->second->comparableStreamDistance * player.radiusMultipliers[STREAMER_TYPE_3D_TEXT_LABEL]);
					}
					else
					{
						distance = Utility::comparableDistance(player.position, Eigen::Vector3f(t->second->position + t->second->positionOffset), t->second->comparableStreamDistance * player.radiusMultipliers[STREAMER_TYPE_3D_TEXT_LABEL]);
					}
				}
			}
			const bool streamed = player.internalTextLabels.contains(t->first);
			if (distance < (t->second->comparableStreamDistance * player.radiusMultipliers[STREAMER_TYPE_3D_TEXT_LABEL]))
			{
				if (!streamed)
				{
					discoveredTextLabels.push_back(Item::Candidate<Item::SharedTextLabel>(t->second->priority, distance, t->second));
				}
				else
				{
					if (t->second->cell)
					{
						player.visibleCell->textLabels.insert(*t);
					}
					existingTextLabels.push_back(Item::Candidate<Item::SharedTextLabel>(t->second->priority, distance, t->second));
				}
			}
			else if (streamed)
			{
				std::unordered_map<int, int>::iterator i = player.internalTextLabels.find(t->first);
				if (i != player.internalTextLabels.end())
				{
					sampgdk::DeletePlayer3DTextLabel(player.playerId, i->second);
					if (t->second->streamCallbacks)
					{
						streamOutCallbacks.push_back(std::make_tuple(STREAMER_TYPE_3D_TEXT_LABEL, t->first, player.playerId));
					}
					player.internalTextLabels.erase(i);
				}
			}
		}
	}
	std::sort(discoveredTextLabels.begin(), discoveredTextLabels.end(), Item::CandidateCompare<Item::SharedTextLabel>());
	std::sort(existingTextLabels.begin(), existingTextLabels.end(), Item::CandidateCompare<Item::SharedTextLabel>());
	for (std::vector<Item::Candidate<Item::SharedTextLabel> >::iterator d = discoveredTextLabels.begin(); d != discoveredTextLabels.end(); ++d)
	{
		const Item::SharedTextLabel &textLabel = d->item;
		std::unordered_map<int, int>::iterator i = player.internalTextLabels.find(textLabel->textLabelId);
		if (i != player.internalTextLabels.end())
		{
			continue;
		}
		if (player.internalTextLabels.size() == player.currentVisibleTextLabels)
		{
			// existingTextLabels is sorted ascending under that same
			// comparator, so back() is exactly the old rbegin() element.
			if (!existingTextLabels.empty())
			{
				const Item::Candidate<Item::SharedTextLabel> &e = existingTextLabels.back();
				if (e.priority < d->priority || (e.distance > STREAMER_STATIC_DISTANCE_CUTOFF && d->distance < e.distance))
				{
					std::unordered_map<int, int>::iterator j = player.internalTextLabels.find(e.item->textLabelId);
					if (j != player.internalTextLabels.end())
					{
						sampgdk::DeletePlayer3DTextLabel(player.playerId, j->second);
						if (e.item->streamCallbacks)
						{
							streamOutCallbacks.push_back(std::make_tuple(STREAMER_TYPE_3D_TEXT_LABEL, e.item->textLabelId, player.playerId));
						}
						player.internalTextLabels.erase(j);
					}
					if (e.item->cell)
					{
						player.visibleCell->textLabels.erase(e.item->textLabelId);
					}
					existingTextLabels.pop_back();
				}
			}
		}
		if (player.internalTextLabels.size() == player.maxVisibleTextLabels)
		{
			player.currentVisibleTextLabels = player.internalTextLabels.size();
			break;
		}
		int internalId = sampgdk::CreatePlayer3DTextLabel(player.playerId, textLabel->text.c_str(), textLabel->color, textLabel->position[0], textLabel->position[1], textLabel->position[2], textLabel->drawDistance, textLabel->attach ? textLabel->attach->player : INVALID_PLAYER_ID, textLabel->attach ? textLabel->attach->vehicle : INVALID_VEHICLE_ID, textLabel->testLOS);
		if (internalId == INVALID_3DTEXT_ID)
		{
			player.currentVisibleTextLabels = player.internalTextLabels.size();
			break;
		}
		if (textLabel->streamCallbacks)
		{
			streamInCallbacks.push_back(std::make_tuple(STREAMER_TYPE_3D_TEXT_LABEL, textLabel->textLabelId, player.playerId));
		}
		player.internalTextLabels.insert(std::make_pair(textLabel->textLabelId, internalId));
		if (textLabel->cell)
		{
			player.visibleCell->textLabels.insert(std::make_pair(textLabel->textLabelId, textLabel));
		}
	}
}

void Streamer::processActiveItems()
{
	if (!movingObjects.empty())
	{
		processMovingObjects();
	}
	if (!attachedAreas.empty())
	{
		processAttachedAreas();
	}
	if (!attachedObjects.empty())
	{
		processAttachedObjects();
	}
	if (!attachedTextLabels.empty())
	{
		processAttachedTextLabels();
	}
}

void Streamer::processMovingObjects()
{
	// All objects in this pass advance against the same clock read; reading it
	// once removes a steady_clock::now() per moving object per pass.
	std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
	std::unordered_set<Item::SharedObject>::iterator o = movingObjects.begin();
	while (o != movingObjects.end())
	{
		bool objectFinishedMoving = false;
		if ((*o)->move)
		{
			std::chrono::duration<float, std::milli> elapsedTime = currentTime - (*o)->move->time;
			if (std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count() < (*o)->move->duration)
			{
				(*o)->position = std::get<1>((*o)->move->position) + (std::get<2>((*o)->move->position) * elapsedTime.count());
				if (!Utility::almostEquals(std::get<0>((*o)->move->rotation).maxCoeff(), -1000.0f))
				{
					(*o)->rotation = std::get<1>((*o)->move->rotation) + (std::get<2>((*o)->move->rotation) * elapsedTime.count());
				}
			}
			else
			{
				(*o)->position = std::get<0>((*o)->move->position);
				if (!Utility::almostEquals(std::get<0>((*o)->move->rotation).maxCoeff(), -1000.0f))
				{
					(*o)->rotation = std::get<0>((*o)->move->rotation);
				}
				(*o)->move.reset();
				objectMoveCallbacks.push_back((*o)->objectId);
				objectFinishedMoving = true;
			}
			// Only re-index when the object actually crosses a cell boundary;
			// the old code erased and re-inserted it in the grid on every pass
			// even though it stayed in the same cell.
			core->getGrid()->updateObjectCell(*o);
		}
		if (objectFinishedMoving)
		{
			o = movingObjects.erase(o);
		}
		else
		{
			++o;
		}
	}
}

void Streamer::processAttachedAreas()
{
	// Anchors are constant within one pass. Areas attached to the same player
	// or vehicle used to re-fetch the anchor (and, for vehicles, scan every
	// player to decide whether the vehicle is occupied) once per area. Cache
	// the per-anchor data for the duration of the pass instead.
	struct PlayerAnchor
	{
		float position[3];
		float heading;
		bool valid;
	};
	struct VehicleAnchor
	{
		float position[3];
		float heading;
		float rotation[4];
		bool occupied;
		bool valid;
	};
	std::unordered_map<int, PlayerAnchor> playerAnchors;
	std::unordered_map<int, VehicleAnchor> vehicleAnchors;
	std::unordered_set<int> occupiedVehicles;
	bool occupancyBuilt = false;
	for (std::unordered_set<Item::SharedArea>::iterator a = attachedAreas.begin(); a != attachedAreas.end(); ++a)
	{
		if ((*a)->attach)
		{
			bool adjust = false;
			if ((std::get<0>((*a)->attach->object) != INVALID_OBJECT_ID && std::get<1>((*a)->attach->object) != STREAMER_OBJECT_TYPE_DYNAMIC) || (std::get<0>((*a)->attach->object) != INVALID_STREAMER_ID && std::get<1>((*a)->attach->object) == STREAMER_OBJECT_TYPE_DYNAMIC))
			{
				switch (std::get<1>((*a)->attach->object))
				{
					case STREAMER_OBJECT_TYPE_GLOBAL:
					{
						Eigen::Vector3f position = Eigen::Vector3f::Zero(), rotation = Eigen::Vector3f::Zero();
						adjust = sampgdk::GetObjectPos(std::get<0>((*a)->attach->object), &position[0], &position[1], &position[2]);
						sampgdk::GetObjectRot(std::get<0>((*a)->attach->object), &rotation[0], &rotation[1], &rotation[2]);
						Utility::constructAttachedArea(*a, std::variant<float, Eigen::Vector3f, Eigen::Vector4f>(rotation), position);
						break;
					}
					case STREAMER_OBJECT_TYPE_PLAYER:
					{
						Eigen::Vector3f position = Eigen::Vector3f::Zero(), rotation = Eigen::Vector3f::Zero();
						adjust = sampgdk::GetPlayerObjectPos(std::get<2>((*a)->attach->object), std::get<0>((*a)->attach->object), &position[0], &position[1], &position[2]);
						sampgdk::GetPlayerObjectRot(std::get<2>((*a)->attach->object), std::get<0>((*a)->attach->object), &rotation[0], &rotation[1], &rotation[2]);
						Utility::constructAttachedArea(*a, std::variant<float, Eigen::Vector3f, Eigen::Vector4f>(rotation), position);
						break;
					}
					case STREAMER_OBJECT_TYPE_DYNAMIC:
					{
						std::unordered_map<int, Item::SharedObject>::iterator o = core->getData()->objects.find(std::get<0>((*a)->attach->object));
						if (o != core->getData()->objects.end())
						{
							Utility::constructAttachedArea(*a, std::variant<float, Eigen::Vector3f, Eigen::Vector4f>(o->second->rotation), o->second->position);
							adjust = true;
						}
						break;
					}
				}
			}
			else if ((*a)->attach->player != INVALID_PLAYER_ID)
			{
				std::unordered_map<int, PlayerAnchor>::iterator cached = playerAnchors.find((*a)->attach->player);
				if (cached == playerAnchors.end())
				{
					PlayerAnchor anchor;
					anchor.position[0] = 0.0f;
					anchor.position[1] = 0.0f;
					anchor.position[2] = 0.0f;
					anchor.heading = 0.0f;
					anchor.valid = sampgdk::GetPlayerPos((*a)->attach->player, &anchor.position[0], &anchor.position[1], &anchor.position[2]);
					sampgdk::GetPlayerFacingAngle((*a)->attach->player, &anchor.heading);
					cached = playerAnchors.insert(std::make_pair((*a)->attach->player, anchor)).first;
				}
				adjust = cached->second.valid;
				Utility::constructAttachedArea(*a, std::variant<float, Eigen::Vector3f, Eigen::Vector4f>(cached->second.heading), Eigen::Vector3f(cached->second.position[0], cached->second.position[1], cached->second.position[2]));
			}
			else if ((*a)->attach->vehicle != INVALID_VEHICLE_ID)
			{
				std::unordered_map<int, VehicleAnchor>::iterator cached = vehicleAnchors.find((*a)->attach->vehicle);
				if (cached == vehicleAnchors.end())
				{
					VehicleAnchor anchor;
					anchor.position[0] = 0.0f;
					anchor.position[1] = 0.0f;
					anchor.position[2] = 0.0f;
					anchor.heading = 0.0f;
					anchor.rotation[0] = 0.0f;
					anchor.rotation[1] = 0.0f;
					anchor.rotation[2] = 0.0f;
					anchor.rotation[3] = 0.0f;
					if (!occupancyBuilt)
					{
						// Resolve occupancy for every vehicle once per pass instead
						// of once per vehicle-attached area. The old per-area loop
						// was O(areas * players) native calls, and it could not
						// break early for a parked vehicle because no driver was
						// found.
						for (std::unordered_map<int, Player>::iterator p = core->getData()->players.begin(); p != core->getData()->players.end(); ++p)
						{
							if (sampgdk::GetPlayerState(p->first) == PLAYER_STATE_DRIVER)
							{
								occupiedVehicles.insert(sampgdk::GetPlayerVehicleID(p->first));
							}
						}
						occupancyBuilt = true;
					}
					anchor.occupied = occupiedVehicles.find((*a)->attach->vehicle) != occupiedVehicles.end();
					anchor.valid = sampgdk::GetVehiclePos((*a)->attach->vehicle, &anchor.position[0], &anchor.position[1], &anchor.position[2]);
					if (!anchor.occupied)
					{
						sampgdk::GetVehicleZAngle((*a)->attach->vehicle, &anchor.heading);
					}
					else
					{
						sampgdk::GetVehicleRotationQuat((*a)->attach->vehicle, &anchor.rotation[0], &anchor.rotation[1], &anchor.rotation[2], &anchor.rotation[3]);
					}
					cached = vehicleAnchors.insert(std::make_pair((*a)->attach->vehicle, anchor)).first;
				}
				adjust = cached->second.valid;
				Eigen::Vector3f position(cached->second.position[0], cached->second.position[1], cached->second.position[2]);
				if (!cached->second.occupied)
				{
					Utility::constructAttachedArea(*a, std::variant<float, Eigen::Vector3f, Eigen::Vector4f>(cached->second.heading), position);
				}
				else
				{
					Eigen::Vector4f quaternion(cached->second.rotation[0], cached->second.rotation[1], cached->second.rotation[2], cached->second.rotation[3]);
					Utility::constructAttachedArea(*a, std::variant<float, Eigen::Vector3f, Eigen::Vector4f>(quaternion), position);
				}
			}
			if (adjust)
			{
				if ((*a)->cell)
				{
					core->getGrid()->removeArea(*a, true);
				}
			}
			else
			{
				switch ((*a)->type)
				{
					case STREAMER_AREA_TYPE_CIRCLE:
					case STREAMER_AREA_TYPE_CYLINDER:
					{
						std::get<Eigen::Vector2f>((*a)->attach->position).fill(std::numeric_limits<float>::infinity());
						break;
					}
					case STREAMER_AREA_TYPE_SPHERE:
					{
						std::get<Eigen::Vector3f>((*a)->attach->position).fill(std::numeric_limits<float>::infinity());
						break;
					}
					case STREAMER_AREA_TYPE_RECTANGLE:
					{
						std::get<Box2d>((*a)->attach->position).min_corner().fill(std::numeric_limits<float>::infinity());
						std::get<Box2d>((*a)->attach->position).max_corner().fill(std::numeric_limits<float>::infinity());
						break;
					}
					case STREAMER_AREA_TYPE_CUBOID:
					{
						std::get<Box3d>((*a)->attach->position).min_corner().fill(std::numeric_limits<float>::infinity());
						std::get<Box3d>((*a)->attach->position).max_corner().fill(std::numeric_limits<float>::infinity());
						break;
					}
					case STREAMER_AREA_TYPE_POLYGON:
					{
						std::get<Polygon2d>((*a)->attach->position).clear();
						break;
					}
				}
			}
		}
	}
}

void Streamer::processAttachedObjects()
{
	// Anchor transforms are constant within one pass. Objects attached to the
	// same player/vehicle used to fetch the anchor position and virtual world
	// once per object; with thousands of attachments that was thousands of
	// native calls per pass. Cache them per anchor instead.
	struct Anchor
	{
		Eigen::Vector3f position;
		int world;
		bool valid;
	};
	std::unordered_map<int, Anchor> playerAnchors;
	std::unordered_map<int, Anchor> vehicleAnchors;
	for (std::unordered_set<Item::SharedObject>::iterator o = attachedObjects.begin(); o != attachedObjects.end(); ++o)
	{
		if ((*o)->attach)
		{
			bool adjust = false;
			if ((*o)->attach->object != INVALID_STREAMER_ID)
			{
				std::unordered_map<int, Item::SharedObject>::iterator p = core->getData()->objects.find((*o)->attach->object);
				if (p != core->getData()->objects.end())
				{
					(*o)->attach->position = p->second->position;
					(*o)->attach->worlds = p->second->worlds;
					adjust = true;
				}
			}
			else if ((*o)->attach->player != INVALID_PLAYER_ID)
			{
				std::unordered_map<int, Anchor>::iterator cached = playerAnchors.find((*o)->attach->player);
				if (cached == playerAnchors.end())
				{
					Anchor anchor;
					anchor.position.setZero();
					anchor.valid = sampgdk::GetPlayerPos((*o)->attach->player, &anchor.position[0], &anchor.position[1], &anchor.position[2]);
					anchor.world = sampgdk::GetPlayerVirtualWorld((*o)->attach->player);
					cached = playerAnchors.insert(std::make_pair((*o)->attach->player, anchor)).first;
				}
				(*o)->attach->position = cached->second.position;
				Utility::setFirstValueInContainer((*o)->attach->worlds, cached->second.world);
				adjust = cached->second.valid;
			}
			else if ((*o)->attach->vehicle != INVALID_VEHICLE_ID)
			{
				std::unordered_map<int, Anchor>::iterator cached = vehicleAnchors.find((*o)->attach->vehicle);
				if (cached == vehicleAnchors.end())
				{
					Anchor anchor;
					anchor.position.setZero();
					anchor.valid = sampgdk::GetVehiclePos((*o)->attach->vehicle, &anchor.position[0], &anchor.position[1], &anchor.position[2]);
					anchor.world = sampgdk::GetVehicleVirtualWorld((*o)->attach->vehicle);
					cached = vehicleAnchors.insert(std::make_pair((*o)->attach->vehicle, anchor)).first;
				}
				(*o)->attach->position = cached->second.position;
				Utility::setFirstValueInContainer((*o)->attach->worlds, cached->second.world);
				adjust = cached->second.valid;
			}
			if (adjust)
			{
				// Re-index only when the anchor moved the object to another cell.
				core->getGrid()->updateObjectCell(*o);
			}
			else
			{
				(*o)->attach->position.fill(std::numeric_limits<float>::infinity());
			}
		}
	}
}

void Streamer::processAttachedTextLabels()
{
	// Same per-anchor cache as processAttachedObjects: labels attached to the
	// same player/vehicle share one anchor transform per pass.
	struct Anchor
	{
		Eigen::Vector3f position;
		int world;
		bool valid;
	};
	std::unordered_map<int, Anchor> playerAnchors;
	std::unordered_map<int, Anchor> vehicleAnchors;
	for (std::unordered_set<Item::SharedTextLabel>::iterator t = attachedTextLabels.begin(); t != attachedTextLabels.end(); ++t)
	{
		bool adjust = false;
		if ((*t)->attach)
		{
			if ((*t)->attach->player != INVALID_PLAYER_ID)
			{
				std::unordered_map<int, Anchor>::iterator cached = playerAnchors.find((*t)->attach->player);
				if (cached == playerAnchors.end())
				{
					Anchor anchor;
					anchor.position.setZero();
					anchor.valid = sampgdk::GetPlayerPos((*t)->attach->player, &anchor.position[0], &anchor.position[1], &anchor.position[2]);
					anchor.world = sampgdk::GetPlayerVirtualWorld((*t)->attach->player);
					cached = playerAnchors.insert(std::make_pair((*t)->attach->player, anchor)).first;
				}
				(*t)->attach->position = cached->second.position;
				Utility::setFirstValueInContainer((*t)->attach->worlds, cached->second.world);
				adjust = cached->second.valid;
			}
			else if ((*t)->attach->vehicle != INVALID_VEHICLE_ID)
			{
				std::unordered_map<int, Anchor>::iterator cached = vehicleAnchors.find((*t)->attach->vehicle);
				if (cached == vehicleAnchors.end())
				{
					Anchor anchor;
					anchor.position.setZero();
					anchor.valid = sampgdk::GetVehiclePos((*t)->attach->vehicle, &anchor.position[0], &anchor.position[1], &anchor.position[2]);
					anchor.world = sampgdk::GetVehicleVirtualWorld((*t)->attach->vehicle);
					cached = vehicleAnchors.insert(std::make_pair((*t)->attach->vehicle, anchor)).first;
				}
				(*t)->attach->position = cached->second.position;
				Utility::setFirstValueInContainer((*t)->attach->worlds, cached->second.world);
				adjust = cached->second.valid;
			}
			if (adjust)
			{
				// Re-index only when the anchor moved the label to another cell.
				core->getGrid()->updateTextLabelCell(*t);
			}
			else
			{
				(*t)->attach->position.fill(std::numeric_limits<float>::infinity());
			}
		}
	}
}
