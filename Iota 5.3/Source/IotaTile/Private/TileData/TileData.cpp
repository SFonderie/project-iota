// Copyright Sydney Fonderie. All Rights Reserved.

#include "TileData/TileData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileData)

FTileData::FTileData()
{
	// Default constructor.
}

FTileData::FTileData(const TSoftObjectPtr<UWorld>& InLevel, const TArray<FTilePortal>& InPortals, const TArray<FTileBound>& InBounds)
	: Level(InLevel)
	, Portals(InPortals)
	, Bounds(InBounds)
{
	// Complete constructor.
}

FTileData::FTileData(const FTileData& TileData, const FTransform& Transform)
	: Level(TileData.Level)
{
	for (const FTilePortal& Portal : TileData.Portals)
	{
		Portals.Emplace(Portal, Transform);
	}

	for (const FTileBound& Bound : TileData.Bounds)
	{
		Bounds.Emplace(Bound, Transform);
	}
}
