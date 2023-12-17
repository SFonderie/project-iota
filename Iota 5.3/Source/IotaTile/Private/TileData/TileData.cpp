// Copyright Sydney Fonderie. All Rights Reserved.

#include "TileData/TileData.h"
#include "TileData/TileDataAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileData)

FTileData::FTileData(const UTileDataAsset* InDataAsset)
	: Level(InDataAsset->Level)
	, Portals(InDataAsset->Portals)
	, Bounds(InDataAsset->Bounds)
{
	// Complete constructor.
}

FTileData FTileData::WithTransform(const FTransform& Transform) const
{
	FTileData NewTile;
	NewTile.Level = Level;
	NewTile.Portals.Empty(Portals.Num());
	NewTile.Bounds.Empty(Bounds.Num());

	for (const FTilePortal& Portal : Portals)
	{
		NewTile.Portals.Emplace(Portal, Transform);
	}

	for (const FTileBound& Bound : Bounds)
	{
		NewTile.Bounds.Emplace(Bound, Transform);
	}

	return NewTile;
}
