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
