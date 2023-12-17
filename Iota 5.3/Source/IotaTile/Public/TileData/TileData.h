// Copyright Sydney Fonderie. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TileData/TileBound.h"
#include "TileData/TilePortal.h"
#include "TileData.generated.h"

class UTileDataAsset;

/** Minimal data structure needed to physically describe a tile. */
USTRUCT(BlueprintType)
struct IOTATILE_API FTileData
{
	GENERATED_BODY()

public:

	/** Pointer to the tile level asset. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UWorld> Level;

	/** A list of the tile's exit portals. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FTilePortal> Portals;

	/** A list of the tile's collision bounds. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FTileBound> Bounds;

	/** Defines an empty tile data structure. */
	FTileData() = default;

	/**
	 * Defines a tile data structure from the provided data asset base.
	 *
	 * @param InDataAsset Tile data asset from which to build.
	 */
	FTileData(const UTileDataAsset* InDataAsset);

	/**
	 * Transforms the tile data and returns the result.
	 *
	 * @param Transform Transform to be applied to the new data.
	 * @return Transformed tile data.
	 */
	FTileData WithTransform(const FTransform& Transform) const;
};
