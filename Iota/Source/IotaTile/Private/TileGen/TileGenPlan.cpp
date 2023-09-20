// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGen/TileGenPlan.h"
#include "TileGen/TileGenData.h"

FPlanPortal::FPlanPortal(const FTilePortal& BasePortal, const FTransform& Transform)
	: FTilePortal(BasePortal, Transform)
{
	// Complete constructor.
}

FPlanPortal::FPlanPortal(const FPlanPortal& PlanPortal, const FTransform& Transform)
	: FTilePortal(PlanPortal, Transform)
	, ConnectionIndex(PlanPortal.ConnectionIndex)
{
	// Copy constructor.
}

FTileGenPlan::FTileGenPlan()
	: Location(0, 0, 0)
	, Rotation(0, 0, 0)
{
	// Default constructor.
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

FTileGenPlan::FTileGenPlan(const FTileGenPlan& TilePlan, const FTransform& Transform)
	: Level(TilePlan.Level)
	, Location(Transform.TransformPosition(TilePlan.Location))
	, Rotation(Transform.TransformRotation(TilePlan.Rotation.Quaternion()).Rotator())
{
	for (const FPlanPortal& Portal : TilePlan.Portals)
	{
		Portals.Emplace(Portal, Transform);
	}

	for (const FTileBound& Bound : TilePlan.Bounds)
	{
		Bounds.Emplace(Bound, Transform);
	}
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
