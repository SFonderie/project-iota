// Copyright Sydney Fonderie. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TilePlan.generated.h"

/** Minimal data structure needed to load a planned tile. */
USTRUCT(BlueprintType)
struct IOTATILE_API FTilePlan
{
	GENERATED_BODY()

public:

	/** Pointer to the tile level asset. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UWorld> Level;

	/** Tile level location in world space. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Location;

	/** Tile level rotation in world space. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator Rotation;

	/** Defines a default tile plan. */
	FTilePlan();

	/**
	 * Defines a new tile plan.
	 *
	 * @param InLevel Soft pointer to the tile level to actually load.
	 * @param InLocation Tile level location in world space.
	 * @param InRotation Tile level rotation in world space.
	 */
	FTilePlan(const TSoftObjectPtr<UWorld>& InLevel, const FVector& InLocation, const FRotator& InRotation);

	/**
	 * Duplicates the given tile plan.
	 *
	 * @param TilePlan Tile plan to duplicate.
	 */
	FTilePlan(const FTilePlan& TilePlan);
};
