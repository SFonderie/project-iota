// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TileData/TileBound.h"
#include "TileData/TilePortal.h"

class UTileDataAsset;

/** UTileDataAsset thread-safe proxy. */
struct FTileGenData
{
	/** Path to the tile level instance. */
	FSoftObjectPath Level;

	/** List of tile portals. */
	TArray<FTilePortal> Portals;

	/** List of tile collision bounds. */
	TArray<FTileBound> Bounds;

	/** Defines an empty data struct. */
	FTileGenData();

	/** Copies values from the given tile data asset. */
	FTileGenData(const UTileDataAsset* DataAsset);

	/** Copies values from the given tile data struct. */
	FTileGenData(const FTileGenData& DataStruct);
};
