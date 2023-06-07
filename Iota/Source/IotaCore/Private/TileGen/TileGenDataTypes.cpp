// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGen/TileGenDataTypes.h"
#include "TileData/TileDataAsset.h"

FTileData::FTileData()
{
	// Default constructor.
}

FTileData::FTileData(const UTileDataAsset* InAsset)
	: Level(InAsset->Level.ToSoftObjectPath())
	, Portals(InAsset->Portals)
	, Bounds(InAsset->Bounds)
{
	// Complete constructor.
}

FTileData::FTileData(const FTileData& InTileData)
	: Level(InTileData.Level)
	, Portals(InTileData.Portals)
	, Bounds(InTileData.Bounds)
{
	// Copy constructor.
}

bool FTileDataSelector::GetRandomTile(const FRandomStream& InRandom, FTileData& OutTileData, int32& OutIndex) const
{
	if (DataBuffer.Num() - SelectionFilter <= DuplicateFilter)
	{
		return false;
	}

	OutIndex = InRandom.RandRange(DuplicateFilter, DataBuffer.Num() - SelectionFilter - 1);
	OutTileData = DataBuffer[OutIndex];
	return true;
}

void FTileDataSelector::AcceptIndex(int32 Index, int32 DuplicationBuffer)
{
	check(DuplicateFilter <= Index && Index <= DataBuffer.Num() - SelectionFilter - 1);

	// The duplicate filter is at the front of the array, so an element can be moved there by
	// swapping it and the first non-filtered element and incrementing the filter length.
	DataBuffer.Swap(DuplicateFilter, Index);
	++DuplicateFilter;

	while (DuplicateFilter > DuplicationBuffer)
	{
		// If the duplicate filter would overflow, the oldest element needs to be released. 
		// Filtered elements are ordered chronologically, so the highest-index element needs 
		// to be swapped out and then the filter shrunk.
		DataBuffer.Swap(0, DuplicateFilter);
		--DuplicateFilter;

		// Now the newest element is at the oldest index, so it should be shifted up the array.
		for (int32 i = 0; i < DuplicateFilter - 1; i++)
		{
			DataBuffer.Swap(i, i + 1);
		}
	}

	// When a tile is accepted, the selection filter should be reset to release rejected tiles
	// back into the tile pool.
	SelectionFilter = 0;
}

void FTileDataSelector::RejectIndex(int32 Index)
{
	check(DuplicateFilter <= Index && Index <= DataBuffer.Num() - SelectionFilter - 1);

	// The selection filter is at the back of the array, so an element can be moved there by
	// swapping it and the last non-filtered element and incrementing the filter length.
	DataBuffer.Swap(Index, DataBuffer.Num() - SelectionFilter - 1);
	++SelectionFilter;
}
