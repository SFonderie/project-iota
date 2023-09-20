// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TileGenParams.generated.h"

enum class ETileScheme : uint8;

/** Defines parameters for level generation. */
USTRUCT(BlueprintType)
struct IOTATILE_API FTileGenParams
{
	GENERATED_BODY()

public:

	/** Tileset that should be used to generate the new level. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Tileset"))
	FGameplayTag Tileset = FGameplayTag::RequestGameplayTag(TEXT("Tileset.Whitebox"));

	/**
	 * Primary objective type for the generated level. Only tiles marked with this exact objective
	 * type will be considered for the objective tile palette.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Objective"))
	FGameplayTag MainObjective = FGameplayTag::RequestGameplayTag(TEXT("Objective.Explore"));

	/**
	 * Secondary objective types for the generated level. Tiles marked with these objectives will
	 * be included in non-objective tile palettes.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Objective"))
	FGameplayTagContainer SideObjectives;

	/** Global origin point for the generated level. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Location;

	/** Global rotation applied to the generated level. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator Rotation;

	/**
	 * Number of main objective tiles to place in the level.
	 *
	 * @warning If this value exceeds the number of objective tiles actually in the tileset, then
	 * level generation will fail.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ClampMin = 0))
	int32 ObjectiveCount = 0;

	/**
	 * Length of the generated level measured in non-terminal tiles. Note that branching tiles are
	 * included in this count, so the distance between the start and end may be shorter.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ClampMin = 0))
	int32 Length = 10;

	/** Maximum branch length for the generated level. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ClampMin = 0))
	int32 Branch = 1;

	/** Random seed value with which to generate the level. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Seed = 0;

	/** Create a default parameter set. */
	FTileGenParams();

	/** Copies values from the given parameter set. */
	FTileGenParams(const FTileGenParams& Params);

	/** Calculates the sequence of tile schemes defined by the parameters. */
	void GetTileSequence(TArray<ETileScheme>& OutSequence) const;
};
