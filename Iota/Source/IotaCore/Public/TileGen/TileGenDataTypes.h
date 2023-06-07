// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TileData/TilePortal.h"
#include "TileData/TileBound.h"

class UTileDataAsset;

/** Thread-cached copy of UTileDataAsset. */
struct IOTACORE_API FTileData
{
	/** Path to the actual tile level instance. */
	FSoftObjectPath Level;

	/** A list of the tile's exit portals. */
	TArray<FTilePortal> Portals;

	/** A list of the tile's collision bounds. */
	TArray<FTileBound> Bounds;

	/** Defines an empty data object. */
	FTileData();

	/** Copies values from the given tile data asset. */
	FTileData(const UTileDataAsset* InAsset);

	/** Copies values from the given tile data object. */
	FTileData(const FTileData& InTileData);
};

/** Templated array wrapper with random utility methods. */
template <typename InElementType>
struct IOTACORE_API TSelectorArray
{
	/** Type used by the selector array. */
	typedef InElementType ElementType;

	/** Selector element array. */
	TArray<ElementType> Elements;

	/** Exclusion at the back of the array. */
	int32 SelectionFilter = 0;

	/** Exclusion at the front of the array. */
	int32 DuplicateFilter = 0;

	/**
	 * Attempts to randomly select an element from the selector's active range.
	 *
	 * @param InRandom Random stream with which to select an element.
	 * @param OutElement Returns the selected element here, if applicable.
	 * @param OutIndex Returns the selected index here, if applicable.
	 * @return Whether there are any elements left to select.
	 */
	bool GetRandom(const FRandomStream& InRandom, ElementType& OutElement, int32& OutIndex) const;

	/**
	 * Accepts the element at the given index. Doing so moves the element into the duplicate
	 * filter, preventing it from being selected again on subsequent selection cycles.
	 *
	 * @param Index Target index.
	 * @param DuplicationBuffer Maximum cycles an element can remain in the duplicate filter.
	 */
	void AcceptIndex(int32 Index, int32 DuplicationBuffer = 1);

	/**
	 * Rejects the element at the given index. Doing so moves the element into the selection
	 * filter, preventing it from being selected again during this selection cycle.
	 *
	 * @param Index Target index.
	 */
	void RejectIndex(int32 Index);
};

template <typename ElementType>
bool TSelectorArray<ElementType>::GetRandom(const FRandomStream& InRandom, ElementType& OutElement, int32& OutIndex) const
{
	if (Elements.Num() - SelectionFilter <= DuplicateFilter)
	{
		return false;
	}

	// Randomly select an index using the given random stream and the two filter values.
	OutIndex = InRandom.RandRange(DuplicateFilter, Elements.Num() - SelectionFilter - 1);
	OutElement = DataBuffer[OutIndex];
	return true;
}

template <typename ElementType>
void TSelectorArray<ElementType>::AcceptIndex(int32 Index, int32 DuplicationBuffer)
{
	check(DuplicateFilter <= Index && Index <= Elements.Num() - SelectionFilter - 1);

	// The duplicate filter is at the front of the array, so an element can be moved there by
	// swapping it and the first non-filtered element and incrementing the filter length.
	Elements.Swap(DuplicateFilter, Index);
	++DuplicateFilter;

	while (DuplicateFilter > DuplicationBuffer)
	{
		// If the duplicate filter would overflow, the oldest element needs to be released. 
		// Filtered elements are ordered chronologically, so the highest-index element needs 
		// to be swapped out and then the filter shrunk.
		Elements.Swap(0, DuplicateFilter);
		--DuplicateFilter;

		// Now the newest element is at the oldest index, so it should be shifted up the array.
		for (int32 i = 0; i < DuplicateFilter - 1; i++)
		{
			Elements.Swap(i, i + 1);
		}
	}

	// When an element is accepted, the selection filter should be reset to release rejected 
	// elements back into the active range.
	SelectionFilter = 0;
}

template <typename ElementType>
void TSelectorArray<ElementType>::RejectIndex(int32 Index)
{
	check(DuplicateFilter <= Index && Index <= DataBuffer.Num() - SelectionFilter - 1);

	// The selection filter is at the back of the array, so an element can be moved there by
	// swapping it and the last non-filtered element and incrementing the filter length.
	DataBuffer.Swap(Index, DataBuffer.Num() - SelectionFilter - 1);
	++SelectionFilter;
}
