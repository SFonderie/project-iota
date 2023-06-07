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

/** Tile data array wrapper with some access utility methods. */
struct IOTACORE_API FTileDataSelector
{
	/** Selector tile data array. */
	TArray<FTileData> DataBuffer;

	/** Exclusion at the back of the array. */
	int32 SelectionFilter = 0;

	/** Exclusion at the front of the array. */
	int32 DuplicateFilter = 0;

	/**
	 * Attempts to randomly select a tile from the selector's active range.
	 *
	 * @param InRandom Random stream with which to select a tile.
	 * @param OutTileData Returns the selected tile data here, if applicable.
	 * @param OutIndex Returns the selected index here, if applicable.
	 * @return Whether there are any tiles left to select.
	 */
	bool GetRandomTile(const FRandomStream& InRandom, FTileData& OutTileData, int32& OutIndex) const;

	/**
	 * Accepts the tile at the given index. Doing so moves the tile into the duplicate filter,
	 * preventing it from being selected again on subsequent selection cycles.
	 *
	 * @param Index Target index.
	 * @param DuplicationBuffer Maximum cycles a tile can remain in the duplicate filter.
	 */
	void AcceptIndex(int32 Index, int32 DuplicationBuffer = 1);

	/**
	 * Rejects the tile at the given index. Doing so moves the tile into the selection filter,
	 * preventing it from being selected again during this selection cycle.
	 *
	 * @param Index Target index.
	 */
	void RejectIndex(int32 Index);
};
