// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGen/TileGraphPlan.h"
#include "TileData/TileData.h"

FTileGraphPortal::FTileGraphPortal(const FTilePortal& BasePortal, const FTransform& Transform)
	: FTilePortal(BasePortal, Transform)
{
	// Complete constructor.
}

FTileGraphPlan::FTileGraphPlan(const FTileData& TemplateData, const FTransform& Transform)
	: FTilePlan(TemplateData.Level, Transform.GetLocation(), Transform.Rotator())
{
	for (const FTilePortal& Portal : TemplateData.Portals)
	{
		Portals.Emplace(Portal, Transform);
	}

	for (const FTileBound& Bound : TemplateData.Bounds)
	{
		Bounds.Emplace(Bound, Transform);
	}
}

void FTileGraphPlan::SetConnection(int32 Index, int32 GraphIndex, bool bParent)
{
	check(0 <= Index && Index < Portals.Num());
	Portals[Index].ConnectionIndex = GraphIndex;

	if (bParent)
	{
		Portals.Swap(0, Index);
	}
}

int32 FTileGraphPlan::GetConnection(int32 Index) const
{
	check(0 <= Index && Index < Portals.Num());
	return Portals[Index].ConnectionIndex;
}

bool FTileGraphPlan::IsOpenPortal(int32 Index) const
{
	check(0 <= Index && Index < Portals.Num());
	return Portals[Index].ConnectionIndex < 0;
}
