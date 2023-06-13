// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TileData/TilePortal.h"
#include "TileData/TileBound.h"
#include "TileData/TilePlan.h"
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

	/** Size of the level not including any stopper tiles. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Size = 16;

	/** Maximum possible branch length when not otherwise restricted. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Branch = 4;

	/** Generates the new level using this seed value. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Seed = 0;

	/** Objective types to include in the level tile subset. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Objective"))
	FGameplayTagContainer Objectives;

	/** Number of objective-related tiles to place in the level. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Complexity = 0;

	/** Creates an empty parameter set. */
	FTileGenParams();

	/** Copies values from the given generation parameters. */
	FTileGenParams(const FTileGenParams& InParams);
};

/** Thread-cached copy of UTileDataAsset. */
struct FTileGenData
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

/** 
 * Extension of Tile Gen Data that includes a world transform and tile state information like
 * portal states. When generation completes, these planned tiles are exported to standard tile
 * plan structs and then replicated via the Game State.
 */
struct FTileGenPlan : public FTileGenData
{
	/** Tile world position. */
	FVector Position;

	/** Tile world rotation. */
	FRotator Rotation;

	/** Bitmask tracking portal index state. 0 values are open, 1 values are closed. */
	uint32 PortalsMask = 0;

	/** Index of the parent tile plan array element. Used to traverse the tile tree. */
	int32 ParentIndex = -1;

	/** 
	 * Copies values from the given tile data object and transform to create a world tile plan.
	 * 
	 * @param InTileData Base tile data with which to build the plan.
	 * @param InTransform World transform to apply to the tile data components.
	 */
	FTileGenPlan(const FTileGenData& InTileData, const FTransform& InTransform);

	/** @return True if the portal at the given index is marked open. */
	bool IsOpenPortal(int32 Index) const;

	/** Marks the portal at the given index open. */
	void OpenPortal(int32 Index);

	/** Marks the portal at the given index closed. */
	void ClosePortal(int32 Index);

	/** @return Exports the generation plan to a tile plan. */
	FTilePlan GetTilePlan() const
	{
		return FTilePlan(TSoftObjectPtr<UWorld>(Level), Position, Rotation);
	}
};
