// Copyright Sydney Fonderie. All Rights Reserved.

#include "TileData/TilePlan.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TilePlan)

FTilePlan::FTilePlan(const TSoftObjectPtr<UWorld>& InLevel, const FVector& InLocation, const FRotator& InRotation)
	: Level(InLevel)
	, Location(InLocation)
	, Rotation(InRotation)
{
	// Complete constructor.
}
