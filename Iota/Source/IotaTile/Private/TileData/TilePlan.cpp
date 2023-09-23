// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileData/TilePlan.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TilePlan)

FTilePlan::FTilePlan()
	: Location(0, 0, 0)
	, Rotation(0, 0, 0)
{
	// Default constructor.
}

FTilePlan::FTilePlan(const TSoftObjectPtr<UWorld>& InLevel, const FVector& InLocation, const FRotator& InRotation)
	: Level(InLevel)
	, Location(InLocation)
	, Rotation(InRotation)
{
	// Complete constructor.
}
