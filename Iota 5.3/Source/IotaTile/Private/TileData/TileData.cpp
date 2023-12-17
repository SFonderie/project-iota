// Copyright Sydney Fonderie. All Rights Reserved.

#include "TileData/TileData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileData)

FTileData::FTileData()
	: Level()
	, Portals()
	, Bounds()
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

FTileData::FTileData(FTileData&& TileData) noexcept
	: Level(MoveTemp(TileData.Level))
	, Portals(MoveTemp(TileData.Portals))
	, Bounds(MoveTemp(TileData.Bounds))
{
	// Move constructor.
}

FTileData& FTileData::operator=(const FTileData& TileData)
{
	if (this != &TileData)
	{
		Level = TileData.Level;
		Portals = TileData.Portals;
		Bounds = TileData.Bounds;
	}

	return *this;
}

FTileData& FTileData::operator=(FTileData&& TileData) noexcept
{
	Level = MoveTemp(TileData.Level);
	Portals = MoveTemp(TileData.Portals);
	Bounds = MoveTemp(TileData.Bounds);
	return *this;
}
