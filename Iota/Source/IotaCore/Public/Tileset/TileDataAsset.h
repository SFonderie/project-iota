// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TileDataAsset.generated.h"

/** Defines different tile functions within a tileset. */
UENUM(BlueprintType)
enum class ETileScheme : uint8
{
	/**
	 * Indicates that the tile should be used as the level start point.
	 * Start tiles must have at least one portal.
	 */
	Start,

	/**
	 * Indicates that the tile should be used to connect other major tiles.
	 * Connector tiles must have at least two portals.
	 */
	Connector,

	/**
	 * Indicates that the tile should be used as a major branch or arena.
	 * Intermediate tiles must have at least three portals.
	 */
	Intermediate,

	/**
	 * Indicates that the tile contains some kind of level objective.
	 * Objective tiles must have at least two portals.
	 */
	Objective,

	/**
	 * Indicates that the tile should be used to close-off empty portals.
	 * Stopper tiles can have at most one portal.
	 */
	Stopper,

	/**
	 * Indicates that the tile should be used as the level exit point.
	 * Exit tiles must have at least one portal.
	 */
	Exit,
};

/** Container describing a tile. */
UCLASS()
class IOTACORE_API UTileDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	/** Pointer to the actual tile level instance. */
	UPROPERTY(Category = "Level", EditAnywhere)
	TSoftObjectPtr<class UWorld> Level;

	/** Preferred tile scheme. */
	UPROPERTY(Category = "Level", EditAnywhere)
	ETileScheme Scheme = ETileScheme::Stopper;

	/** A list of the tile's exit portals. */
	UPROPERTY(Category = "Description", EditAnywhere)
	TArray<struct FTilePortal> Portals;

	/** A list of the tile's summary collision bounds. */
	UPROPERTY(Category = "Description", EditAnywhere)
	TArray<struct FTileBound> Bounds;
};
