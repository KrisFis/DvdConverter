
#pragma once
#include "SafeInt.h"

namespace sal
{
	
	// Struct used by allocators
	// Its for remembering allocated blocks
	// Is necessary for deallocation
	struct FBlockManager
	{
		FBlockManager(const uint32& InLenght);

		virtual ~FBlockManager();

		// Checks whether index exist in array
		// @return - if searching was successful
		bool IndexUsed(const uint32& InIndex) const;

		// Tries to find empty index (not used)
		// @param - OUT found free index
		// @return - whether searching operation was successful
		bool FindEmptyIndex(uint32& OutFreeIndex) const;

		// Adds index to free block
		// @param - index to add
		// @param - checks whether index already exists [Default: true]
		// @return - if adding was successful
		bool AddIndex(const uint32& InIndex, bool CheckExistence = true);

		// Removes index from block
		// Find first occurrence of block with specific index and erase it
		// @param - specific index to remove
		// @return - if remove was successful
		bool RemoveIndex(const uint32& InIndex);

		uint32* Blocks;
		uint32 Length;

		static const uint32 EMPTY_BLOCK = M_UINT32;
	};

}