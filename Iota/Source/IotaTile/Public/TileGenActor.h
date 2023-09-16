// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "TileGenActor.generated.h"

/** Base class for asset-searchable actors used in level generation. */
UCLASS(Abstract)
class IOTATILE_API ATileGenActor : public AActor
{
	GENERATED_BODY()

public:

	/** @return Asset ID that makes child actor types visible to the Asset Manager. */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	/** Tilesets to which this actor belongs. Used to filter candidates during generation. */
	UPROPERTY(Category = "AssetID", EditDefaultsOnly, meta = (Categories = "Tileset"))
	FGameplayTagContainer Tilesets;
};
