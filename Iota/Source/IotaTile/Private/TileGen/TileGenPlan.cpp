// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGen/TileGenPlan.h"
#include "TileGen/TileGenData.h"

FPlanPortal::FPlanPortal(const FTilePortal& BasePortal, const FTransform& Transform)
	: FTilePortal(BasePortal, Transform)
{
	// Complete constructor.
}

FPlanPortal::FPlanPortal(const FPlanPortal& PlanPortal)
	: FTilePortal(PlanPortal)
	, ConnectionIndex(PlanPortal.ConnectionIndex)
{
	// Copy constructor.
}

FTileGenPlan::FTileGenPlan(const FTileGenData& DataStruct, const FTransform& Transform)
	: Level(DataStruct.Level)
	, Location(Transform.GetLocation())
	, Rotation(Transform.GetRotation())
{
	for (const FTilePortal& Portal : DataStruct.Portals)
	{
		Portals.Emplace(Portal, Transform);
	}

	for (const FTileBound& Bound : DataStruct.Bounds)
	{
		Bounds.Emplace(Bound, Transform);
	}
}

FTileGenPlan::FTileGenPlan(const FTileGenPlan& TilePlan)
	: Level(TilePlan.Level)
	, Portals(TilePlan.Portals)
	, Bounds(TilePlan.Bounds)
	, Location(TilePlan.Location)
	, Rotation(TilePlan.Rotation)
{
	// Copy constructor.
}

void FTileGenPlan::SetConnection(int32 Index, int32 GraphIndex, bool bParent)
{
	check(0 <= Index && Index < Portals.Num());
	Portals[Index].ConnectionIndex = GraphIndex;

	if (bParent)
	{
		Portals.Swap(0, Index);
	}
}

int32 FTileGenPlan::GetConnection(int32 Index) const
{
	check(0 <= Index && Index < Portals.Num());
	return Portals[Index].ConnectionIndex;
}

bool FTileGenPlan::IsOpenPortal(int32 Index) const
{
	check(0 <= Index && Index < Portals.Num());
	return Portals[Index].ConnectionIndex < 0;
}
