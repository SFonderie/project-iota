// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TileDataAsset.generated.h"

struct FTilePortal;
struct FTileBound;

/** Defines different tile functions within a tileset. */
UENUM(BlueprintType)
enum class ETileScheme : uint8
{
	/**
	 * Indicates that a tile should be used to determine a level spawn point.
	 * Start tiles must have at least one portal.
	 */
	Start,

	/**
	 * Indicates that a tile should be used to bridge between other tiles.
	 * Connection tiles must have at least two portals.
	 */
	Connection,

	/**
	 * Indicates that a tile should be used to branch the level or as an arena.
	 * Intermediate tiles must have at least three portals.
	 */
	Intermediate,

	/**
	 * Indicates that a tile should be used to indicate a level objective.
	 * Objective tiles must have at least two portals.
	 */
	Objective,

	/**
	 * Indicates that a tile should be used to seal-off empty portals.
	 * Stopper tiles can have at most one portal.
	 */
	Stopper,

	/**
	 * Indicates that a tile should be used to determine a level end point.
	 * Exit tiles must have at least one portal.
	 */
	Exit,
};

/** Primary data asset type describing a tile level. */
UCLASS()
class IOTACORE_API UTileDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/** Pointer to the actual tile level instance. */
	UPROPERTY(Category = "Level", EditAnywhere)
	TSoftObjectPtr<UWorld> Level;

	/** Preferred tile scheme. */
	UPROPERTY(Category = "Level", EditAnywhere)
	ETileScheme Scheme = ETileScheme::Stopper;

	/** A list of the tile's exit portals. */
	UPROPERTY(Category = "Description", EditAnywhere)
	TArray<FTilePortal> Portals;

	/** A list of the tile's summary collision bounds. */
	UPROPERTY(Category = "Description", EditAnywhere)
	TArray<FTileBound> Bounds;
};
