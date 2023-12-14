// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TileGenParams.generated.h"

enum class ETileScheme : uint8;

/** Defines the parameters needed to generate a new tile map. */
USTRUCT(BlueprintType)
struct IOTATILE_API FTileGenParams
{
	GENERATED_BODY()

public:

	/** Tileset to be used when generating the tile map. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Tileset"))
	FGameplayTag Tileset = FGameplayTag::RequestGameplayTag("Tileset.Whitebox");

	/**
	 * Primary objective type to use in the generated tile map. Only tiles marked with this exact
	 * tag will be considered for the objective tile palette.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Objective"))
	FGameplayTag MainObjective = FGameplayTag::RequestGameplayTag("Objective.Explore");

	/**
	 * Secondary objective types to use in the generated tile map. Tiles marked with any of these
	 * tags will be included in non-objective tile palettes.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Objective"))
	FGameplayTagContainer SideObjectives;

	/** World origin point for the generated tile map. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Location;

	/** World rotation for the generated tile map. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator Rotation;

	/** Number of main objective tiles to add to the generated tile map. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ClampMin = 0))
	int32 ObjectiveCount = 0;

	/** Size of the generated tile map measured in non-terminal tiles. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ClampMin = 0))
	int32 Length = 10;

	/** Maximum tree depth to be used when adding a new tile to the tile map. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ClampMin = 0))
	int32 Branch = 1;

	/** Random seed value with which to generate the tile map. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Seed = 0;

	/** List of Asset Actor types to load with the tile map. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FPrimaryAssetType> AssetActors;

	/** Creates a default parameter set. */
	FTileGenParams();

	/** Copies values from the given parameter set. */
	FTileGenParams(const FTileGenParams& Params);

	/** Determines the sequence of tile schemes defined by the parameter set. */
	void GetSchemeSequence(TArray<ETileScheme>& OutSequence) const;
};
