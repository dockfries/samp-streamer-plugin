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

#ifndef PLAYER_H
#define PLAYER_H

#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "cell.h"
#include "identifier.h"

// Internal ("player object") id table for one item type. The map stays the
// source of truth; a bitmap mirrors its keys so the streaming hot path can test
// membership in O(1) without hashing every candidate in the player's cells.
// All mutations go through insert/erase/clear, which keeps both in sync.
class InternalIdMap
{
public:
	typedef std::unordered_map<int, int>::iterator iterator;
	typedef std::unordered_map<int, int>::const_iterator const_iterator;

	iterator begin()
	{
		return entries.begin();
	}

	iterator end()
	{
		return entries.end();
	}

	const_iterator begin() const
	{
		return entries.begin();
	}

	const_iterator end() const
	{
		return entries.end();
	}

	iterator find(int id)
	{
		return entries.find(id);
	}

	const_iterator find(int id) const
	{
		return entries.find(id);
	}

	bool contains(int id) const
	{
		if (id < 0)
		{
			return false;
		}
		std::size_t word = static_cast<std::size_t>(id) >> 6;
		return word < bits.size() && (bits[word] & (std::uint64_t(1) << (id & 63))) != 0;
	}

	std::pair<iterator, bool> insert(const std::pair<int, int> &value)
	{
		setBit(value.first);
		return entries.insert(value);
	}

	iterator erase(iterator position)
	{
		clearBit(position->first);
		return entries.erase(position);
	}

	std::size_t erase(int id)
	{
		clearBit(id);
		return entries.erase(id);
	}

	std::size_t size() const
	{
		return entries.size();
	}

	bool empty() const
	{
		return entries.empty();
	}

	void clear()
	{
		entries.clear();
		std::fill(bits.begin(), bits.end(), 0);
	}
private:
	void setBit(int id)
	{
		if (id < 0)
		{
			return;
		}
		std::size_t word = static_cast<std::size_t>(id) >> 6;
		if (word >= bits.size())
		{
			bits.resize(word + 1, 0);
		}
		bits[word] |= std::uint64_t(1) << (id & 63);
	}

	void clearBit(int id)
	{
		if (id < 0)
		{
			return;
		}
		std::size_t word = static_cast<std::size_t>(id) >> 6;
		if (word < bits.size())
		{
			bits[word] &= ~(std::uint64_t(1) << (id & 63));
		}
	}

	std::unordered_map<int, int> entries;
	std::vector<std::uint64_t> bits;
};

struct Player
{
	Player(int id);

	int activeCheckpoint;
	int activeRaceCheckpoint;
	std::size_t chunkTickCount[STREAMER_MAX_TYPES];
	std::size_t chunkTickRate[STREAMER_MAX_TYPES];
	std::size_t currentVisibleObjects;
	std::size_t currentVisibleTextLabels;
	int delayedCheckpoint;
	int delayedRaceCheckpoint;
	bool delayedUpdate;
	bool delayedUpdateFreeze;
	std::chrono::steady_clock::time_point delayedUpdateTime;
	int delayedUpdateType;
	int interiorId;
	std::size_t maxVisibleMapIcons;
	std::size_t maxVisibleObjects;
	std::size_t maxVisibleTextLabels;
	int playerId;
	Eigen::Vector3f position;
	float radiusMultipliers[STREAMER_MAX_TYPES];
	bool requestingClass;
	std::size_t tickCount;
	std::size_t tickRate;
	bool updatedThisTick;
	bool updateUsingCameraPosition;
	bool updateWhenIdle;
	SharedCell visibleCell;
	int visibleCheckpoint;
	int visibleRaceCheckpoint;
	int worldId;

	std::bitset<STREAMER_MAX_TYPES> enabledItems;
	std::bitset<STREAMER_MAX_TYPES> processingChunks;

	Item::Bimap<Item::SharedMapIcon>::Type discoveredMapIcons;
	Item::Bimap<Item::SharedObject>::Type discoveredObjects;
	Item::Bimap<Item::SharedTextLabel>::Type discoveredTextLabels;

	// Already streamed candidates for the chunked types. These are rebuilt on
	// every player update and only read while draining, so they are kept as
	// reused sorted vectors instead of a bimap: no node allocation per
	// candidate and a much cheaper insertion path.
	std::vector<Item::Candidate<Item::SharedMapIcon> > existingMapIcons;
	std::vector<Item::Candidate<Item::SharedObject> > existingObjects;
	std::vector<Item::Candidate<Item::SharedTextLabel> > existingTextLabels;

	// Scratch candidate lists for the non-chunk streaming passes. Held on the
	// player rather than rebuilt per update so those passes stop allocating a
	// fresh container (and a tree node per candidate) every single time.
	std::vector<Item::Candidate<Item::SharedCheckpoint> > discoveredCheckpointCandidates;
	std::vector<Item::Candidate<Item::SharedMapIcon> > discoveredMapIconCandidates;
	std::vector<Item::Candidate<Item::SharedMapIcon> > existingMapIconCandidates;
	std::vector<Item::Candidate<Item::SharedObject> > discoveredObjectCandidates;
	std::vector<Item::Candidate<Item::SharedObject> > existingObjectCandidates;
	std::vector<Item::Candidate<Item::SharedRaceCheckpoint> > discoveredRaceCheckpointCandidates;
	std::vector<Item::Candidate<Item::SharedTextLabel> > discoveredTextLabelCandidates;
	std::vector<Item::Candidate<Item::SharedTextLabel> > existingTextLabelCandidates;

	std::unordered_set<int> internalAreas;
	InternalIdMap internalMapIcons;
	InternalIdMap internalObjects;
	InternalIdMap internalTextLabels;

	std::unordered_set<int> removedMapIcons;
	std::unordered_set<int> removedObjects;
	std::unordered_set<int> removedTextLabels;

	Identifier mapIconIdentifier;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif
