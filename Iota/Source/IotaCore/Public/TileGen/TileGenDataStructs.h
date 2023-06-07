// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TileData/TilePortal.h"
#include "TileData/TileBound.h"
#include "TileGenDataStructs.generated.h"

class UTileDataAsset;

/** Provides parameters for level generation. */
USTRUCT(BlueprintType)
struct IOTACORE_API FTileGenParams
{
	GENERATED_BODY()

public:

	/** Generates the new level using this tileset category. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Tileset"))
	FGameplayTag Tileset = FGameplayTag::RequestGameplayTag(TEXT("Tileset.Whitebox"));

	/** Breadth of the level not including start, exit, or stopper tiles. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Breadth = 16;

	/** Generates the new level using this seed value. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Seed = 0;

	/** Creates an empty parameter set. */
	FTileGenParams();

	/** Copies values from the given generation parameters. */
	FTileGenParams(const FTileGenParams& InParams);
};

/** Thread-cached copy of UTileDataAsset. */
struct IOTACORE_API FTileGenData
{
	/** Path to the actual tile level instance. */
	FSoftObjectPath Level;

	/** A list of the tile's exit portals. */
	TArray<FTilePortal> Portals;

	/** A list of the tile's collision bounds. */
	TArray<FTileBound> Bounds;

	/** Defines an empty data object. */
	FTileGenData();

	/** Copies values from the given tile data asset. */
	FTileGenData(const UTileDataAsset* InAsset);

	/** Copies values from the given tile data object. */
	FTileGenData(const FTileGenData& InData);
};
