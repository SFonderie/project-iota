// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileData/TileGenData.h"
#include "TileData/TileDataAsset.h"
#include "TileData/TilePlan.h"

FTileGenData::FTileGenData()
{
	// Default constructor.
}

FTileGenData::FTileGenData(const UTileDataAsset* DataAsset)
	: Level(DataAsset->Level.ToSoftObjectPath())
	, Portals(DataAsset->Portals)
	, Bounds(DataAsset->Bounds)
{
	// Complete constructor.
}

FTileGenData::FTileGenData(const FTileGenData& TileData)
	: Level(TileData.Level)
	, Portals(TileData.Portals)
	, Bounds(TileData.Bounds)
{
	// Copy constructor.
}

FTileGenPlan::FTileGenPlan(const FTileGenData& TileData, const FTransform& Transform)
	: FTileGenData(TileData)
	, Position(Transform.GetLocation())
	, Rotation(Transform.GetRotation())
{
	for (FTilePortal& Portal : Portals)
	{
		Portal = FTilePortal(Portal, Transform);
	}

	for (FTileBound& Bound : Bounds)
	{
		Bound = FTileBound(Bound, Transform);
	}
}

bool FTileGenPlan::IsOpenPortal(int32 Index) const
{
	check(0 <= Index && Index < sizeof(PortalsMask) * 8);
	return (PortalsMask & (1 << Index)) == 0;
}

void FTileGenPlan::OpenPortal(int32 Index)
{
	check(0 <= Index && Index < sizeof(PortalsMask) * 8);
	PortalsMask &= PortalsMask ^ (1 << Index);
}

void FTileGenPlan::ClosePortal(int32 Index)
{
	check(0 <= Index && Index < sizeof(PortalsMask) * 8);
	PortalsMask |= (1 << Index);
}

FTilePlan FTileGenPlan::GetTilePlan() const
{
	return FTilePlan(TSoftObjectPtr<UWorld>(Level), Position, Rotation);
}
