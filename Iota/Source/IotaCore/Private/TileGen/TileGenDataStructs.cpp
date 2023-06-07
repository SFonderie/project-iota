// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGen/TileGenDataStructs.h"
#include "TileData/TileDataAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileGenDataStructs)

FTileGenParams::FTileGenParams()
{
	// Default constructor.
}

FTileGenParams::FTileGenParams(const FTileGenParams& InParams)
	: Tileset(InParams.Tileset)
	, Breadth(InParams.Breadth)
	, Seed(InParams.Seed)
{
	// Copy constructor.
}

FTileGenData::FTileGenData()
{
	// Default constructor.
}

FTileGenData::FTileGenData(const UTileDataAsset* InAsset)
	: Level(InAsset->Level.ToSoftObjectPath())
	, Portals(InAsset->Portals)
	, Bounds(InAsset->Bounds)
{
	// Complete constructor.
}

FTileGenData::FTileGenData(const FTileGenData& InTileData)
	: Level(InTileData.Level)
	, Portals(InTileData.Portals)
	, Bounds(InTileData.Bounds)
{
	// Copy constructor.
}
