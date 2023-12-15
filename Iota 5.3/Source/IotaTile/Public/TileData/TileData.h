// Copyright Sydney Fonderie. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TileData/TileBound.h"
#include "TileData/TilePortal.h"
#include "TileData.generated.h"

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
	FTileData();

	/**
	 * Defines a tile data structure using the provided attributes.
	 *
	 * @param InLevel Pointer to the tile level asset.
	 * @param InPortals A list of the tile's exit portals.
	 * @param InBounds A list of the tile's collision bounds.
	 */
	FTileData(const TSoftObjectPtr<UWorld>& InLevel, const TArray<FTilePortal>& InPortals, const TArray<FTileBound>& InBounds);

	/**
	 * Duplicates the given tile data and optionally transforms its components.
	 *
	 * @param Tile Data Tile data structure to duplicate.
	 * @param Transform World transform to apply to the data.
	 */
	FTileData(const FTileData& TileData, const FTransform& Transform = FTransform::Identity);
};
