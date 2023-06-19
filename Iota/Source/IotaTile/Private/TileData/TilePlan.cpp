// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileData/TilePlan.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TilePlan)

FTilePlan::FTilePlan()
	: Level(nullptr)
	, Position(0, 0, 0)
	, Rotation(0, 0, 0)
{
	// Default constructor.
}

FTilePlan::FTilePlan(const TSoftObjectPtr<UWorld>& InLevel, const FVector& InPosition, const FRotator& InRotation)
	: Level(InLevel)
	, Position(InPosition)
	, Rotation(InRotation)
{
	// Complete constructor.
}

FTilePlan::FTilePlan(const FTilePlan& TilePlan, const FTransform& Transform)
	: Level(TilePlan.Level)
	, Position(Transform.TransformPosition(TilePlan.Position))
	, Rotation(Transform.TransformRotation(TilePlan.Rotation.Quaternion()).Rotator())
{
	// Copy constructor.
}
