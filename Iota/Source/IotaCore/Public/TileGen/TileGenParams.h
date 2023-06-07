// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TileGenParams.generated.h"

/** Provides parameters for level generation. */
USTRUCT(BlueprintType)
struct IOTACORE_API FTileGenParams
{
	GENERATED_BODY()

public:

	/** Generates the new level using this tileset category. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "Tileset"))
	FGameplayTag Tileset = FGameplayTag::RequestGameplayTag(TEXT("Tileset.Whitebox"));

	/** Generates the new level using this seed value. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Seed = 0;
};
