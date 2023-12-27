// Copyright Sydney Fonderie. All Rights Reserved.

#include "TileData/TileGraphPlan.h"
#include "TileData/TileDataAsset.h"

FTileGraphData::FTileGraphData(const UTileDataAsset* InDataAsset)
	: FTileData(InDataAsset)
	, AreaNodes(InDataAsset->AreaNodes)
	, AreaExits(InDataAsset->AreaExits)
{
	// Complete constructor.
}

FTileGraphPortal::FTileGraphPortal(const FTilePortal& TilePortal, const FTransform& Transform)
	: FTilePortal(TilePortal, Transform)
{
	// Complete constructor.
}

FTileGraphPlan::FTileGraphPlan(const FTileGraphData& TileData, const FTransform& Transform, bool bRoot)
	: FTilePlan(TileData.Level, Transform.GetLocation(), Transform.Rotator())
{
	if (bRoot)
	{
		// Root tiles do not have parents, so add an empty portal as the parent connection.
		Portals.Emplace(FTilePortal(), Transform);
		Portals.GetData()->TileConnectionIndex = -2;
	}

	for (const FTilePortal& Portal : TileData.Portals)
	{
		// Transform each tile portal into world coordinates.
		int32 Index = Portals.Emplace(Portal, Transform);

		// Place the area exit values into each matching portal.
		check(Index < TileData.AreaExits.Num());
		Portals[Index].AreaConnectionIndex = TileData.AreaExits[Index];
	}

	for (const FTileBound& Bound : TileData.Bounds)
	{
		// Transform each tile bound into world coordinates.
		Bounds.Emplace(Bound, Transform);
	}

	for (const FTileNode& Node : TileData.AreaNodes)
	{
		// Transform each area node into world coordinates.
		AreaNodes.Emplace(Node.WithTransform(Transform));
	}
}

void FTileGraphPlan::SetConnection(int32 Index, int32 MapIndex, bool bParent)
{
	check(0 <= Index && Index < Portals.Num());
	Portals[Index].TileConnectionIndex = MapIndex;

	if (bParent)
	{
		Portals.Swap(0, Index);
	}
}

int32 FTileGraphPlan::GetConnection(int32 Index) const
{
	check(0 <= Index && Index < Portals.Num());
	return Portals[Index].TileConnectionIndex;
}

bool FTileGraphPlan::IsConnectionOpen(int32 Index) const
{
	check(0 <= Index && Index < Portals.Num());
	return Portals[Index].TileConnectionIndex == -1;
}
