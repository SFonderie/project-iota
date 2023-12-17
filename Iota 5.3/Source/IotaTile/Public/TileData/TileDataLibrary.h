// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TileDataLibrary.generated.h"

struct FTileBound;
struct FTilePortal;
struct FTileData;

class UTileDataAsset;

/** Provides blueprint functions for tile data types. */
UCLASS()
class IOTATILE_API UTileDataLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Determines if the given tile bounds are colliding.
	 *
	 * @param A Tile bound to check.
	 * @param B Tile bound to check.
	 * @return Whether the tile bounds are colliding.
	 */
	UFUNCTION(BlueprintCallable, Category = "Tile|TileBound")
	static bool TileBoundCollision(const FTileBound& A, const FTileBound& B);

	/**
	 * Transforms the given tile bound and returns the result.
	 *
	 * @param TileBound Tile bound to transform.
	 * @param Transform Transform to apply to the tile bound.
	 * @return Transformed tile bound.
	 */
	UFUNCTION(BlueprintPure, Category = "Tile|TileBound")
	static FTileBound TransformTileBound(const FTileBound& TileBound, const FTransform& Transform);

	/**
	 * Calculates the transform needed to position a new tile within a tile map such that it
	 * connects via the given tile portals.
	 *
	 * @param TilePortal Connection portal from the tile level.
	 * @param WorldPortal Connection portal from the world.
	 * @return The level-to-world transform that would connect the tiles.
	 */
	UFUNCTION(BlueprintPure, Category = "Tile|TilePortal")
	static FTransform ConnectionTransform(const FTilePortal& TilePortal, const FTilePortal& WorldPortal);

	/**
	 * Transforms the given tile portal and returns the result.
	 *
	 * @param TilePortal Tile portal to transform.
	 * @param Transform Transform to apply to the tile portal.
	 * @return Transformed tile portal.
	 */
	UFUNCTION(BlueprintPure, Category = "Tile|TilePortal")
	static FTilePortal TransformTilePortal(const FTilePortal& TilePortal, const FTransform& Transform);

	/**
	 * Extracts a tile data structure from the given tile data asset.
	 *
	 * @param DataAsset Tile data asset from which to extract tile data.
	 * @return Structure for the tile the asset represents.
	 */
	UFUNCTION(BlueprintPure, Category = "Tile")
	static FTileData GetTileData(const UTileDataAsset* DataAsset);

	/**
	 * Determines if the given tiles are colliding.
	 *
	 * @param A Tile to check.
	 * @param B Tile to check.
	 * @return Whether the tiles are colliding.
	 */
	UFUNCTION(BlueprintCallable, Category = "Tile")
	static bool TileCollision(const FTileData& A, const FTileData& B);

	/**
	 * Transforms the given tile and returns the result.
	 *
	 * @param TileData Tile to transform.
	 * @param Transform Transform to apply to the tile.
	 * @return Transformed tile.
	 */
	UFUNCTION(BlueprintPure, Category = "Tile")
	static FTileData TransformTile(const FTileData& TileData, const FTransform& Transform);
};
