// Copyright Sydney Fonderie. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "TileAssetActor.generated.h"

/**
 * Tile Asset Actors are actors whose types are meant to be accessed dynamically through the Asset
 * Manager and Asset Actor Tables. They provide an implementation of GetPrimaryAssetId that returns
 * for each Blueprint child class. Each Blueprint can then be accessed in code via its UClass.
 */
UCLASS()
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
	virtual FPrimaryAssetId GetPrimaryAssetId() const override final;

	/**
	 * Categories to which this actor belongs. Category tags are arbitrary and can be left blank
	 * if a child type does not fit into any clear categories.
	 *
	 * Categories can be used to filter Asset Actors during load by accessing and reading their
	 * values from UClass::GetDefaultObject.
	 */
	UPROPERTY(Category = "AssetID", EditDefaultsOnly)
	FGameplayTagContainer Categories;
};
