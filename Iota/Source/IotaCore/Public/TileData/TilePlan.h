// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TileData/TilePortal.h"
#include "TileData/TileBound.h"
#include "TilePlan.generated.h"

/** Structure representing a tile planned by level generation. */
USTRUCT(BlueprintType)
struct IOTACORE_API FTilePlan
{
	GENERATED_BODY()

public:

	/** Tile level instance pointer. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UWorld> Level;

	/** Tile level world position. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Position;

	/** Tile level world rotation. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator Rotation;

	/** Creates an empty tile plan. */
	FTilePlan();

	/** 
	 * Creates a standard tile plan.
	 * 
	 * @param InLevel Soft object pointer to the planned tile level.
	 * @param InPosition Planned position of the tile level in world space.
	 * @param InRotation Planned rotation of the tile level in world space.
	 */
	FTilePlan(const TSoftObjectPtr<UWorld>& InLevel, const FVector& InPosition, const FRotator& InRotation);

	/**
	 * Duplicates the given tile plan and optionally transforms it.
	 * 
	 * @param TilePlan Tile plan to duplicate.
	 * @param Transform Transform to be applied to the new plan.
	 */
	FTilePlan(const FTilePlan& TilePlan, const FTransform& Transform = FTransform::Identity);
};
