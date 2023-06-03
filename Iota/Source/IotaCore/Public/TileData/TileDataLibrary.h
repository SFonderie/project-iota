// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TileDataLibrary.generated.h"

struct FTileBound;
struct FTilePortal;

/** Provides blueprint functions for tile data objects. */
UCLASS()
class IOTACORE_API UTileDataLibrary : public UBlueprintFunctionLibrary
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
	 * Calculates the transform needed to position a level tile into a generated world through the
	 * connection between the given tile portals.
	 *
	 * @param TilePortal Connection portal from the level tile.
	 * @param WorldPortal Connection portal from the world.
	 * @return The level-tile-to-world transform.
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
};
