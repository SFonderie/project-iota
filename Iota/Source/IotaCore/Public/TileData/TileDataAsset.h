// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TileData/TileBound.h"
#include "TileData/TilePortal.h"
#include "GameplayTagContainer.h"
#include "TileDataAsset.generated.h"

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

	/**
	 * Internal enum value used to create static arrays.
	 */
	Count UMETA(Hidden),
};

/** Primary data asset type describing a tile level. */
UCLASS()
class IOTACORE_API UTileDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/** Pointer to the actual tile level instance. */
	UPROPERTY(Category = "TileLevel", EditAnywhere)
	TSoftObjectPtr<UWorld> Level;

	/** Preferred tile scheme. */
	UPROPERTY(Category = "TileLevel", EditAnywhere)
	ETileScheme Scheme = ETileScheme::Stopper;

	/** A list of the tile's exit portals. */
	UPROPERTY(Category = "Attributes", EditAnywhere)
	TArray<FTilePortal> Portals;

	/** A list of the tile's collision bounds. */
	UPROPERTY(Category = "Attributes", EditAnywhere)
	TArray<FTileBound> Bounds;

	/** Tileset to which the tile belongs. */
	UPROPERTY(AssetRegistrySearchable, Category = "TileData", EditAnywhere, meta = (Categories = "Tileset"))
	FGameplayTag Tileset = FGameplayTag::RequestGameplayTag(TEXT("Tileset.Whitebox"));
};
