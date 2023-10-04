// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TilePlan.generated.h"

class UPackageMap;

/** Data needed to stream in a tile level. */
USTRUCT(BlueprintType)
struct IOTATILE_API FTilePlan
{
	GENERATED_BODY()

public:

	/** Pointer to the tile level asset. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UWorld> Level;

	/** Tile level location in world space. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Location;

	/** Tile level rotation in world space. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator Rotation;

	/** Defines a default tile plan. */
	FTilePlan();

	/**
	 * Defines a new tile plan.
	 *
	 * @param InLevel Soft pointer to the tile level to actually load.
	 * @param InLocation Tile level location in world space.
	 * @param InRotation Tile level rotation in world space.
	 */
	FTilePlan(const TSoftObjectPtr<UWorld>& InLevel, const FVector& InLocation, const FRotator& InRotation);

	/**
	 * Duplicates the provided tile plan or tile generation plan.
	 *
	 * @param TilePlan Tile plan to duplicate.
	 */
	FTilePlan(const FTilePlan& TilePlan);

	/**
	 * Compresses the tile plan to minimize its network size.
	 *
	 * @param Archive Archive object from which to read and write.
	 * @param PackageMap Package map for resolving UObject pointers.
	 * @param bOutSuccess True if no errors were encountered.
	 * @return True if the tile plan was fully serialized.
	 */
	bool NetSerialize(FArchive& Archive, UPackageMap* PackageMap, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FTilePlan> : public TStructOpsTypeTraitsBase2<FTilePlan>
{
	enum
	{
		WithNetSerializer = true
	};
};
