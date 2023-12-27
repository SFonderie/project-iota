// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TileData/TileNode.h"
#include "TileData/TileBound.h"
#include "TileData/TileData.h"
#include "TileData/TilePortal.h"
#include "TileData/TileScheme.h"
#include "GameplayTagContainer.h"
#include "TileDataAsset.generated.h"

/** Primary data asset type describing a tile level. */
UCLASS()
class UTileDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/** Pointer to the tile level asset. */
	UPROPERTY(Category = "TileLevel", EditAnywhere)
	TSoftObjectPtr<UWorld> Level;

	/** Preferred tile schemes. */
	UPROPERTY(Category = "TileLevel", EditAnywhere, meta = (Bitmask, BitmaskEnum = ETileScheme))
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

	/** A list of the tile's area graph nodes. */
	UPROPERTY(Category = "Attributes", EditAnywhere)
	TArray<FTileNode> AreaNodes;

	/** Area graph indices nearest to each portal. */
	UPROPERTY()
	TArray<int32> AreaExits;
};
