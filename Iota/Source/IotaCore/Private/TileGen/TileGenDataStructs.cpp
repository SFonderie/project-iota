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
	, Size(InParams.Size)
	, Branch(InParams.Branch)
	, Seed(InParams.Seed)
	, Objectives(InParams.Objectives)
	, Complexity(InParams.Complexity)
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

FTileGenPlan::FTileGenPlan(const FTileGenData& InTileData, const FTransform& InTransform)
	: FTileGenData(InTileData)
	, Position(InTransform.GetLocation())
	, Rotation(InTransform.GetRotation())
{
	for (FTilePortal& Portal : Portals)
	{
		Portal = FTilePortal(Portal, InTransform);
	}

	for (FTileBound& Bound : Bounds)
	{
		Bound = FTileBound(Bound, InTransform);
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
