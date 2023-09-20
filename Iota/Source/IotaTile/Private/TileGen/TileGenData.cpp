// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGen/TileGenData.h"
#include "TileData/TileDataAsset.h"

FTileGenData::FTileGenData(const UTileDataAsset* DataAsset)
	: Level(DataAsset->Level.ToSoftObjectPath())
	, Portals(DataAsset->Portals)
	, Bounds(DataAsset->Bounds)
{
	// Complete constructor.
}

FTileGenData::FTileGenData(const FTileGenData& DataStruct)
	: Level(DataStruct.Level)
	, Portals(DataStruct.Portals)
	, Bounds(DataStruct.Bounds)
{
	// Copy constructor.
}
