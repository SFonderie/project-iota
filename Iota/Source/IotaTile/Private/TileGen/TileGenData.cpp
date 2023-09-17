// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGen/TileGenData.h"
#include "TileData/TileDataAsset.h"

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

FTileGenData::FTileGenData(const FTileGenData& DataStruct)
	: Level(DataStruct.Level)
	, Portals(DataStruct.Portals)
	, Bounds(DataStruct.Bounds)
{
	// Copy constructor.
}

FTileGenPlan::FTileGenPlan(const FTileGenData& DataStruct, const FTransform& Transform)
	: FTileGenData(DataStruct)
	, Position(Transform.GetLocation())
	, Rotation(Transform.GetRotation())
{
	for (FTilePortal& Portal : Portals)
	{
		Portal = FTilePortal(Portal, Transform);
		Connections.Add(-1);
	}

	for (FTileBound& Bound : Bounds)
	{
		Bound = FTileBound(Bound, Transform);
	}
}

void FTileGenPlan::SetConnection(int32 Index, int32 GraphIndex, bool bParent)
{
	check(0 <= Index && Index < Connections.Num());
	Connections[Index] = GraphIndex;

	if (bParent)
	{
		Connections.Swap(0, Index);
		Portals.Swap(0, Index);
	}
}

int32 FTileGenPlan::GetConnection(int32 Index) const
{
	check(0 <= Index && Index < Connections.Num());
	return Connections[Index];
}

bool FTileGenPlan::IsPortalOpen(int32 Index) const
{
	check(0 <= Index && Index < Connections.Num());
	return Connections[Index] < 0;
}
