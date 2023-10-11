// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TileData/TileScheme.h"
#include "TileData/TileBound.h"
#include "TileData/TileData.h"
#include "TileData/TilePortal.h"
#include "GameplayTagContainer.h"
#include "TileDataAsset.generated.h"

/** Primary data asset type describing a tile level. */
UCLASS()
class IOTATILE_API UTileDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/** Pointer to the tile level asset. */
	UPROPERTY(Category = "TileLevel", EditAnywhere)
	TSoftObjectPtr<UWorld> Level;

	/** Preferred tile schemes. */
	UPROPERTY(Category = "TileLevel", EditAnywhere, meta = (Bitmask, BitmaskEnum = "/Script/IotaTile.ETileScheme"))
	int32 Schemes = 1 << ETileScheme::Connector;

	/** Tileset to which the tile belongs. */
	UPROPERTY(AssetRegistrySearchable, Category = "Categories", EditAnywhere, meta = (Categories = "Tileset"))
	FGameplayTag Tileset = FGameplayTag::RequestGameplayTag("Tileset.Whitebox");

	/** Objectives to which the tile belongs. If empty, the tile will appear in all objectives. */
	UPROPERTY(Category = "Categories", EditAnywhere, meta = (Categories = "Objective"))
	FGameplayTagContainer Objectives;

	/** A list of the tile's exit portals. */
	UPROPERTY(Category = "Attributes", EditAnywhere)
	TArray<FTilePortal> Portals;

	/** A list of the tile's collision bounds. */
	UPROPERTY(Category = "Attributes", EditAnywhere)
	TArray<FTileBound> Bounds;

	/**
	 * Converts the tile data asset into generation data.
	 *
	 * @return The tile data asset coverted into a struct.
	 */
	UFUNCTION(BlueprintPure, Category = "Tile|TileData")
	FTileData GetTileData() const;
};
