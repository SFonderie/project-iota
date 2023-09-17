// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "TileAssetActor.generated.h"

/** Base class for asset-searchable actors used in level generation. */
UCLASS(Abstract)
class IOTATILE_API ATileAssetActor : public AActor
{
	GENERATED_BODY()

public:

	/**
	 * Generates a unique Asset ID for each Blueprint type derived from this class, allowing them
	 * to be discovered and accessed via the Asset Manager.
	 *
	 * The asset type used for the Asset ID will equal the highest-level native class in the class
	 * hierarchy, or the lowest-level Blueprint class if no native classes could be found.
	 *
	 * This method will only return a meaningful result when called on the Class Default Object of
	 * a Blueprint type that derives from but is not the immediate child of ATileAssetActor.
	 *
	 * @return Asset ID that makes indirect child Blueprint types visible to the Asset Manager.
	 */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	/** Tilesets to which this actor belongs. Used to filter candidates during generation. */
	UPROPERTY(Category = "AssetID", EditDefaultsOnly, meta = (Categories = "Tileset"))
	FGameplayTagContainer Tilesets;
};
