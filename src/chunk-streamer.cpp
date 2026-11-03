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

#include "chunk-streamer.h"
#include "core.h"

ChunkStreamer::ChunkStreamer()
{
	chunkSize[STREAMER_TYPE_OBJECT] = 100;
	chunkSize[STREAMER_TYPE_MAP_ICON] = 100;
	chunkSize[STREAMER_TYPE_3D_TEXT_LABEL] = 100;
	chunkStreamingEnabled = false;
}

std::size_t ChunkStreamer::getChunkSize(int type)
{
	switch (type)
	{
		case STREAMER_TYPE_OBJECT:
		{
			return chunkSize[STREAMER_TYPE_OBJECT];
		}
		case STREAMER_TYPE_MAP_ICON:
		{
			return chunkSize[STREAMER_TYPE_MAP_ICON];
		}
		case STREAMER_TYPE_3D_TEXT_LABEL:
		{
			return chunkSize[STREAMER_TYPE_3D_TEXT_LABEL];
		}
	}
	return 0;
}

bool ChunkStreamer::setChunkSize(int type, std::size_t value)
{
	if (value > 0)
	{
		switch (type)
		{
			case STREAMER_TYPE_OBJECT:
			{
				chunkSize[STREAMER_TYPE_OBJECT] = value;
				return true;
			}
			case STREAMER_TYPE_MAP_ICON:
			{
				chunkSize[STREAMER_TYPE_MAP_ICON] = value;
				return true;
			}
			case STREAMER_TYPE_3D_TEXT_LABEL:
			{
				chunkSize[STREAMER_TYPE_3D_TEXT_LABEL] = value;
				return true;
			}
		}
	}
	return false;
}

void ChunkStreamer::performPlayerChunkUpdate(Player &player, bool automatic)
{
	for (std::vector<int>::const_iterator t = core->getData()->typePriority.begin(); t != core->getData()->typePriority.end(); ++t)
	{
		switch (*t)
		{
			case STREAMER_TYPE_OBJECT:
			{
				if (player.processingChunks[STREAMER_TYPE_OBJECT])
				{
					streamObjects(player, automatic);
				}
				break;
			}
			case STREAMER_TYPE_MAP_ICON:
			{
				if (player.processingChunks[STREAMER_TYPE_MAP_ICON])
				{
					streamMapIcons(player, automatic);
				}
				break;
			}
			case STREAMER_TYPE_3D_TEXT_LABEL:
			{
				if (player.processingChunks[STREAMER_TYPE_3D_TEXT_LABEL])
				{
					streamTextLabels(player, automatic);
				}
				break;
			}
		}
	}
}

void ChunkStreamer::discoverMapIcons(Player &player, const std::vector<SharedCell> &cells)
{
	player.existingMapIcons.clear();
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
					player.discoveredMapIcons.insert(Item::Bimap<Item::SharedMapIcon>::Type::value_type(std::make_tuple(m->second->priority, distance), std::make_tuple(m->first, m->second)));
				}
				else
				{
					if (m->second->cell)
					{
						player.visibleCell->mapIcons.insert(*m);
					}
					player.existingMapIcons.push_back(Item::Candidate<Item::SharedMapIcon>(m->second->priority, distance, m->second));
				}
			}
			else if (streamed)
			{
				player.removedMapIcons.insert(m->first);
			}
		}
	}
	if (!player.discoveredMapIcons.empty() || !player.removedMapIcons.empty())
	{
		player.processingChunks.set(STREAMER_TYPE_MAP_ICON);
	}
	std::make_heap(player.existingMapIcons.begin(), player.existingMapIcons.end(), Item::CandidateCompare<Item::SharedMapIcon>());
}

void ChunkStreamer::streamMapIcons(Player &player, bool automatic)
{
	if (!automatic || ++player.chunkTickCount[STREAMER_TYPE_MAP_ICON] >= player.chunkTickRate[STREAMER_TYPE_MAP_ICON])
	{
		std::size_t chunkCount = 0;
		if (!player.removedMapIcons.empty())
		{
			std::unordered_set<int>::iterator r = player.removedMapIcons.begin();
			while (r != player.removedMapIcons.end())
			{
				if (automatic && ++chunkCount > chunkSize[STREAMER_TYPE_MAP_ICON])
				{
					break;
				}
				std::unordered_map<int, int>::iterator i = player.internalMapIcons.find(*r);
				if (i != player.internalMapIcons.end())
				{
					sampgdk::RemovePlayerMapIcon(player.playerId, i->second);
					std::unordered_map<int, Item::SharedMapIcon>::iterator m = core->getData()->mapIcons.find(*r);
					if (m != core->getData()->mapIcons.end())
					{
						if (m->second->streamCallbacks)
						{
							streamOutCallbacks.push_back(std::make_tuple(STREAMER_TYPE_MAP_ICON, *r, player.playerId));
						}
					}
					player.mapIconIdentifier.remove(i->second, player.internalMapIcons.size());
					player.internalMapIcons.erase(i);
				}
				r = player.removedMapIcons.erase(r);
			}
		}
		else
		{
			Item::Bimap<Item::SharedMapIcon>::Type::left_iterator d = player.discoveredMapIcons.left.begin();
			while (d != player.discoveredMapIcons.left.end())
			{
				if (automatic && ++chunkCount > chunkSize[STREAMER_TYPE_MAP_ICON])
				{
					break;
				}
				std::unordered_map<int, int>::iterator i = player.internalMapIcons.find(std::get<1>(d->second)->mapIconId);
				if (i != player.internalMapIcons.end())
				{
					d = player.discoveredMapIcons.left.erase(d);
					continue;
				}
				if (player.internalMapIcons.size() == player.maxVisibleMapIcons)
				{
					// The candidate vector may still hold entries whose item was
					// destroyed (and possibly had its id reused) by script since
					// the last discovery pass.
					while (!player.existingMapIcons.empty())
					{
						const Item::Candidate<Item::SharedMapIcon> &e = player.existingMapIcons.front();
						std::unordered_map<int, Item::SharedMapIcon>::iterator current = core->getData()->mapIcons.find(e.item->mapIconId);
						if (!player.internalMapIcons.contains(e.item->mapIconId) || current == core->getData()->mapIcons.end() || current->second != e.item)
						{
							std::pop_heap(player.existingMapIcons.begin(), player.existingMapIcons.end(), Item::CandidateCompare<Item::SharedMapIcon>());
							player.existingMapIcons.pop_back();
							continue;
						}
						break;
					}
					if (!player.existingMapIcons.empty())
					{
						const Item::Candidate<Item::SharedMapIcon> &e = player.existingMapIcons.front();
						if (e.priority < std::get<0>(d->first) || (e.distance > STREAMER_STATIC_DISTANCE_CUTOFF && std::get<1>(d->first) < e.distance))
						{
							int existingId = e.item->mapIconId;
							std::unordered_map<int, int>::iterator j = player.internalMapIcons.find(existingId);
							if (j != player.internalMapIcons.end())
							{
								sampgdk::RemovePlayerMapIcon(player.playerId, j->second);
								if (e.item->streamCallbacks)
								{
									streamOutCallbacks.push_back(std::make_tuple(STREAMER_TYPE_MAP_ICON, existingId, player.playerId));
								}
								player.mapIconIdentifier.remove(j->second, player.internalMapIcons.size());
								player.internalMapIcons.erase(j);
							}
							if (e.item->cell)
							{
								player.visibleCell->mapIcons.erase(existingId);
							}
							std::pop_heap(player.existingMapIcons.begin(), player.existingMapIcons.end(), Item::CandidateCompare<Item::SharedMapIcon>());
							player.existingMapIcons.pop_back();
						}
					}
					if (player.internalMapIcons.size() == player.maxVisibleMapIcons)
					{
						player.discoveredMapIcons.clear();
						break;
					}
				}
				int internalId = player.mapIconIdentifier.get();
				sampgdk::SetPlayerMapIcon(player.playerId, internalId, std::get<1>(d->second)->position[0], std::get<1>(d->second)->position[1], std::get<1>(d->second)->position[2], std::get<1>(d->second)->type, std::get<1>(d->second)->color, std::get<1>(d->second)->style);
				if (std::get<1>(d->second)->streamCallbacks)
				{
					streamInCallbacks.push_back(std::make_tuple(STREAMER_TYPE_MAP_ICON, std::get<1>(d->second)->mapIconId, player.playerId));
				}
				player.internalMapIcons.insert(std::make_pair(std::get<1>(d->second)->mapIconId, internalId));
				if (std::get<1>(d->second)->cell)
				{
					player.visibleCell->mapIcons.insert(std::make_pair(std::get<0>(d->second), std::get<1>(d->second)));
				}
				d = player.discoveredMapIcons.left.erase(d);
			}
		}
		player.chunkTickCount[STREAMER_TYPE_MAP_ICON] = 0;
	}
	if (player.discoveredMapIcons.empty() && player.removedMapIcons.empty())
	{
		player.existingMapIcons.clear();
		player.processingChunks.reset(STREAMER_TYPE_MAP_ICON);
	}
}

void ChunkStreamer::discoverObjects(Player &player, const std::vector<SharedCell> &cells)
{
	player.existingObjects.clear();
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
					player.discoveredObjects.insert(Item::Bimap<Item::SharedObject>::Type::value_type(std::make_tuple(o->second->priority, distance), std::make_tuple(o->first, o->second)));
				}
				else
				{
					if (o->second->cell)
					{
						player.visibleCell->objects.insert(*o);
					}
					player.existingObjects.push_back(Item::Candidate<Item::SharedObject>(o->second->priority, distance, o->second));
				}
			}
			else if (streamed)
			{
				player.removedObjects.insert(o->first);
			}
		}
	}
	if (!player.discoveredObjects.empty() || !player.removedObjects.empty())
	{
		player.processingChunks.set(STREAMER_TYPE_OBJECT);
	}
	std::make_heap(player.existingObjects.begin(), player.existingObjects.end(), Item::CandidateCompare<Item::SharedObject>());
}

void ChunkStreamer::streamObjects(Player &player, bool automatic)
{
	if (!automatic || ++player.chunkTickCount[STREAMER_TYPE_OBJECT] >= player.chunkTickRate[STREAMER_TYPE_OBJECT])
	{
		std::size_t chunkCount = 0;
		if (!player.removedObjects.empty())
		{
			std::unordered_set<int>::iterator r = player.removedObjects.begin();
			while (r != player.removedObjects.end())
			{
				if (automatic && ++chunkCount > chunkSize[STREAMER_TYPE_OBJECT])
				{
					break;
				}
				std::unordered_map<int, int>::iterator i = player.internalObjects.find(*r);
				if (i != player.internalObjects.end())
				{
					sampgdk::DestroyPlayerObject(player.playerId, i->second);
					std::unordered_map<int, Item::SharedObject>::iterator o = core->getData()->objects.find(*r);
					if (o != core->getData()->objects.end())
					{
						if (o->second->streamCallbacks)
						{
							streamOutCallbacks.push_back(std::make_tuple(STREAMER_TYPE_OBJECT, *r, player.playerId));
						}
					}
					player.internalObjects.erase(i);
				}
				r = player.removedObjects.erase(r);
			}
		}
		else
		{
			bool streamingCanceled = false;
			Item::Bimap<Item::SharedObject>::Type::left_iterator d = player.discoveredObjects.left.begin();
			while (d != player.discoveredObjects.left.end())
			{
				if (automatic && ++chunkCount > chunkSize[STREAMER_TYPE_OBJECT])
				{
					break;
				}
				std::unordered_map<int, int>::iterator i = player.internalObjects.find(std::get<1>(d->second)->objectId);
				if (i != player.internalObjects.end())
				{
					d = player.discoveredObjects.left.erase(d);
					continue;
				}
				int internalBaseId = INVALID_STREAMER_ID;
				if (std::get<1>(d->second)->attach)
				{
					if (std::get<1>(d->second)->attach->object != INVALID_STREAMER_ID)
					{
						std::unordered_map<int, int>::iterator j = player.internalObjects.find(std::get<1>(d->second)->attach->object);
						if (j == player.internalObjects.end())
						{
							d = player.discoveredObjects.left.erase(d);
							continue;
						}
						internalBaseId = j->second;
					}
				}
				if (player.internalObjects.size() == player.currentVisibleObjects)
				{
					// The candidate vector may still hold entries whose item was
					// destroyed (and possibly had its id reused) by script since
					// the last discovery pass.
					while (!player.existingObjects.empty())
					{
						const Item::Candidate<Item::SharedObject> &e = player.existingObjects.front();
						std::unordered_map<int, Item::SharedObject>::iterator current = core->getData()->objects.find(e.item->objectId);
						if (!player.internalObjects.contains(e.item->objectId) || current == core->getData()->objects.end() || current->second != e.item)
						{
							std::pop_heap(player.existingObjects.begin(), player.existingObjects.end(), Item::CandidateCompare<Item::SharedObject>());
							player.existingObjects.pop_back();
							continue;
						}
						break;
					}
					if (!player.existingObjects.empty())
					{
						const Item::Candidate<Item::SharedObject> &e = player.existingObjects.front();
						if (e.priority < std::get<0>(d->first) || (e.distance > STREAMER_STATIC_DISTANCE_CUTOFF && std::get<1>(d->first) < e.distance))
						{
							int existingId = e.item->objectId;
							std::unordered_map<int, int>::iterator j = player.internalObjects.find(existingId);
							if (j != player.internalObjects.end())
							{
								sampgdk::DestroyPlayerObject(player.playerId, j->second);
								if (e.item->streamCallbacks)
								{
									streamOutCallbacks.push_back(std::make_tuple(STREAMER_TYPE_OBJECT, existingId, player.playerId));
								}
								player.internalObjects.erase(j);
							}
							if (e.item->cell)
							{
								player.visibleCell->objects.erase(existingId);
							}
							std::pop_heap(player.existingObjects.begin(), player.existingObjects.end(), Item::CandidateCompare<Item::SharedObject>());
							player.existingObjects.pop_back();
						}
					}
				}
				if (player.internalObjects.size() == player.maxVisibleObjects)
				{
					streamingCanceled = true;
					break;
				}
				int internalId = sampgdk::CreatePlayerObject(player.playerId, std::get<1>(d->second)->modelId, std::get<1>(d->second)->position[0], std::get<1>(d->second)->position[1], std::get<1>(d->second)->position[2], std::get<1>(d->second)->rotation[0], std::get<1>(d->second)->rotation[1], std::get<1>(d->second)->rotation[2], std::get<1>(d->second)->drawDistance);
				if (internalId == INVALID_OBJECT_ID)
				{
					streamingCanceled = true;
					break;
				}
				if (std::get<1>(d->second)->streamCallbacks)
				{
					streamInCallbacks.push_back(std::make_tuple(STREAMER_TYPE_OBJECT, std::get<0>(d->second), player.playerId));
				}
				if (std::get<1>(d->second)->attach)
				{
					if (internalBaseId != INVALID_STREAMER_ID)
					{
						static AMX_NATIVE native = sampgdk::FindNative("AttachPlayerObjectToObject");
						if (native != NULL)
						{
							sampgdk::InvokeNative(native, "dddffffffb", player.playerId, internalId, internalBaseId, std::get<1>(d->second)->attach->positionOffset[0], std::get<1>(d->second)->attach->positionOffset[1], std::get<1>(d->second)->attach->positionOffset[2], std::get<1>(d->second)->attach->rotation[0], std::get<1>(d->second)->attach->rotation[1], std::get<1>(d->second)->attach->rotation[2], std::get<1>(d->second)->attach->syncRotation);
						}
					}
					else if (std::get<1>(d->second)->attach->player != INVALID_PLAYER_ID)
					{
						static AMX_NATIVE native = sampgdk::FindNative("AttachPlayerObjectToPlayer");
						if (native != NULL)
						{
							sampgdk::InvokeNative(native, "dddffffffd", player.playerId, internalId, std::get<1>(d->second)->attach->player, std::get<1>(d->second)->attach->positionOffset[0], std::get<1>(d->second)->attach->positionOffset[1], std::get<1>(d->second)->attach->positionOffset[2], std::get<1>(d->second)->attach->rotation[0], std::get<1>(d->second)->attach->rotation[1], std::get<1>(d->second)->attach->rotation[2], 1);
						}
					}
					else if (std::get<1>(d->second)->attach->vehicle != INVALID_VEHICLE_ID)
					{
						sampgdk::AttachPlayerObjectToVehicle(player.playerId, internalId, std::get<1>(d->second)->attach->vehicle, std::get<1>(d->second)->attach->positionOffset[0], std::get<1>(d->second)->attach->positionOffset[1], std::get<1>(d->second)->attach->positionOffset[2], std::get<1>(d->second)->attach->rotation[0], std::get<1>(d->second)->attach->rotation[1], std::get<1>(d->second)->attach->rotation[2]);
					}
				}
				else if (std::get<1>(d->second)->move)
				{
					sampgdk::MovePlayerObject(player.playerId, internalId, std::get<0>(std::get<1>(d->second)->move->position)[0], std::get<0>(std::get<1>(d->second)->move->position)[1], std::get<0>(std::get<1>(d->second)->move->position)[2], std::get<1>(d->second)->move->speed, std::get<0>(std::get<1>(d->second)->move->rotation)[0], std::get<0>(std::get<1>(d->second)->move->rotation)[1], std::get<0>(std::get<1>(d->second)->move->rotation)[2]);
				}
				for (std::unordered_map<int, Item::Object::Material>::iterator m = std::get<1>(d->second)->materials.begin(); m != std::get<1>(d->second)->materials.end(); ++m)
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
				if (std::get<1>(d->second)->noCameraCollision)
				{
					sampgdk::SetPlayerObjectNoCameraCol(player.playerId, internalId);
				}
				player.internalObjects.insert(std::make_pair(std::get<0>(d->second), internalId));
				if (std::get<1>(d->second)->cell)
				{
					player.visibleCell->objects.insert(std::make_pair(std::get<0>(d->second), std::get<1>(d->second)));
				}
				d = player.discoveredObjects.left.erase(d);
			}
			if (streamingCanceled)
			{
				player.currentVisibleObjects = player.internalObjects.size();
				player.discoveredObjects.clear();
			}
		}
		player.chunkTickCount[STREAMER_TYPE_OBJECT] = 0;
	}
	if (player.discoveredObjects.empty() && player.removedObjects.empty())
	{
		player.existingObjects.clear();
		player.processingChunks.reset(STREAMER_TYPE_OBJECT);
	}
}

void ChunkStreamer::discoverTextLabels(Player &player, const std::vector<SharedCell> &cells)
{
	player.existingTextLabels.clear();
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
					player.discoveredTextLabels.insert(Item::Bimap<Item::SharedTextLabel>::Type::value_type(std::make_tuple(t->second->priority, distance), std::make_tuple(t->first, t->second)));
				}
				else
				{
					if (t->second->cell)
					{
						player.visibleCell->textLabels.insert(*t);
					}
					player.existingTextLabels.push_back(Item::Candidate<Item::SharedTextLabel>(t->second->priority, distance, t->second));
				}
			}
			else if (streamed)
			{
				player.removedTextLabels.insert(t->first);
			}
		}
	}
	if (!player.discoveredTextLabels.empty() || !player.removedTextLabels.empty())
	{
		player.processingChunks.set(STREAMER_TYPE_3D_TEXT_LABEL);
	}
	std::make_heap(player.existingTextLabels.begin(), player.existingTextLabels.end(), Item::CandidateCompare<Item::SharedTextLabel>());
}

void ChunkStreamer::streamTextLabels(Player &player, bool automatic)
{
	if (!automatic || ++player.chunkTickCount[STREAMER_TYPE_3D_TEXT_LABEL] >= player.chunkTickRate[STREAMER_TYPE_3D_TEXT_LABEL])
	{
		std::size_t chunkCount = 0;
		if (!player.removedTextLabels.empty())
		{
			std::unordered_set<int>::iterator r = player.removedTextLabels.begin();
			while (r != player.removedTextLabels.end())
			{
				if (automatic && ++chunkCount > chunkSize[STREAMER_TYPE_3D_TEXT_LABEL])
				{
					break;
				}
				std::unordered_map<int, int>::iterator i = player.internalTextLabels.find(*r);
				if (i != player.internalTextLabels.end())
				{
					sampgdk::DeletePlayer3DTextLabel(player.playerId, i->second);
					std::unordered_map<int, Item::SharedTextLabel>::iterator t = core->getData()->textLabels.find(*r);
					if (t != core->getData()->textLabels.end())
					{
						if (t->second->streamCallbacks)
						{
							streamOutCallbacks.push_back(std::make_tuple(STREAMER_TYPE_3D_TEXT_LABEL, *r, player.playerId));
						}
					}
					player.internalTextLabels.erase(i);
				}
				r = player.removedTextLabels.erase(r);
			}
		}
		else
		{
			bool streamingCanceled = false;
			Item::Bimap<Item::SharedTextLabel>::Type::left_iterator d = player.discoveredTextLabels.left.begin();
			while (d != player.discoveredTextLabels.left.end())
			{
				if (automatic && ++chunkCount > chunkSize[STREAMER_TYPE_3D_TEXT_LABEL])
				{
					break;
				}
				std::unordered_map<int, int>::iterator i = player.internalTextLabels.find(std::get<1>(d->second)->textLabelId);
				if (i != player.internalTextLabels.end())
				{
					d = player.discoveredTextLabels.left.erase(d);
					continue;
				}
				if (player.internalTextLabels.size() == player.currentVisibleTextLabels)
				{
					// The candidate vector may still hold entries whose item was
					// destroyed (and possibly had its id reused) by script since
					// the last discovery pass.
					while (!player.existingTextLabels.empty())
					{
						const Item::Candidate<Item::SharedTextLabel> &e = player.existingTextLabels.front();
						std::unordered_map<int, Item::SharedTextLabel>::iterator current = core->getData()->textLabels.find(e.item->textLabelId);
						if (!player.internalTextLabels.contains(e.item->textLabelId) || current == core->getData()->textLabels.end() || current->second != e.item)
						{
							std::pop_heap(player.existingTextLabels.begin(), player.existingTextLabels.end(), Item::CandidateCompare<Item::SharedTextLabel>());
							player.existingTextLabels.pop_back();
							continue;
						}
						break;
					}
					if (!player.existingTextLabels.empty())
					{
						const Item::Candidate<Item::SharedTextLabel> &e = player.existingTextLabels.front();
						if (e.priority < std::get<0>(d->first) || (e.distance > STREAMER_STATIC_DISTANCE_CUTOFF && std::get<1>(d->first) < e.distance))
						{
							int existingId = e.item->textLabelId;
							std::unordered_map<int, int>::iterator j = player.internalTextLabels.find(existingId);
							if (j != player.internalTextLabels.end())
							{
								sampgdk::DeletePlayer3DTextLabel(player.playerId, j->second);
								if (e.item->streamCallbacks)
								{
									streamOutCallbacks.push_back(std::make_tuple(STREAMER_TYPE_3D_TEXT_LABEL, existingId, player.playerId));
								}
								player.internalTextLabels.erase(j);
							}
							if (e.item->cell)
							{
								player.visibleCell->textLabels.erase(existingId);
							}
							std::pop_heap(player.existingTextLabels.begin(), player.existingTextLabels.end(), Item::CandidateCompare<Item::SharedTextLabel>());
							player.existingTextLabels.pop_back();
						}
					}
				}
				if (player.internalTextLabels.size() == player.maxVisibleTextLabels)
				{
					streamingCanceled = true;
					break;
				}
				int internalId = sampgdk::CreatePlayer3DTextLabel(player.playerId, std::get<1>(d->second)->text.c_str(), std::get<1>(d->second)->color, std::get<1>(d->second)->position[0], std::get<1>(d->second)->position[1], std::get<1>(d->second)->position[2], std::get<1>(d->second)->drawDistance, std::get<1>(d->second)->attach ? std::get<1>(d->second)->attach->player : INVALID_PLAYER_ID, std::get<1>(d->second)->attach ? std::get<1>(d->second)->attach->vehicle : INVALID_VEHICLE_ID, std::get<1>(d->second)->testLOS);
				if (internalId == INVALID_3DTEXT_ID)
				{
					streamingCanceled = true;
					break;
				}
				if (std::get<1>(d->second)->streamCallbacks)
				{
					streamInCallbacks.push_back(std::make_tuple(STREAMER_TYPE_3D_TEXT_LABEL, std::get<0>(d->second), player.playerId));
				}
				player.internalTextLabels.insert(std::make_pair(std::get<0>(d->second), internalId));
				if (std::get<1>(d->second)->cell)
				{
					player.visibleCell->textLabels.insert(std::make_pair(std::get<0>(d->second), std::get<1>(d->second)));
				}
				d = player.discoveredTextLabels.left.erase(d);
			}
			if (streamingCanceled)
			{
				player.currentVisibleTextLabels = player.internalTextLabels.size();
				player.discoveredTextLabels.clear();
			}
		}
		player.chunkTickCount[STREAMER_TYPE_3D_TEXT_LABEL] = 0;
	}
	if (player.discoveredTextLabels.empty() && player.removedTextLabels.empty())
	{
		player.existingTextLabels.clear();
		player.processingChunks.reset(STREAMER_TYPE_3D_TEXT_LABEL);
	}
}
