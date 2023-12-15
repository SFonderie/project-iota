// Copyright Sydney Fonderie. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TileBound.generated.h"

/** Simple oriented bounding box implementation for use in tile collision checks. */
USTRUCT(BlueprintType)
struct IOTATILE_API FTileBound
{
	GENERATED_BODY()

public:

	/** Center of the oriented bounding box in world space. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Center;

	/** Oriented bounding box rotation in world space. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator Rotation;

	/** Radial extent of the bounding box in local space. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Extent;

	/** Defines a default axis-aligned tile bound. */
	FTileBound();

	/**
	 * Defines an oriented bounding box tile bound.
	 *
	 * @param InCenter Center of the oriented bounding box in world space.
	 * @param InRotation Oriented bounding box rotation in world space.
	 * @param InExtent Radial extent of the bounding box in local space.
	 */
	FTileBound(const FVector& InCenter, const FRotator& InRotation, const FVector& InExtent);

	/**
	 * Duplicates the given tile bound and optionally transforms it.
	 *
	 * @param TileBound Tile bound to duplicate.
	 * @param Transform Transform to be applied to the new bounds.
	 */
	FTileBound(const FTileBound& TileBound, const FTransform& Transform = FTransform::Identity);

	/** @return Oriented bounding box unit forward vector. */
	FVector GetAxisX() const
	{
		return Rotation.RotateVector(FVector::UnitX());
	}

	/** @return Oriented bounding box unit right vector. */
	FVector GetAxisY() const
	{
		return Rotation.RotateVector(FVector::UnitY());
	}

	/** @return Oriented bounding box unit up vector. */
	FVector GetAxisZ() const
	{
		return Rotation.RotateVector(FVector::UnitZ());
	}

	/** @return True if the tile bounds intersect each other. */
	bool IsCollidingWith(const FTileBound& Other) const
	{
		return CheckCollision(*this, Other);
	}

	/** Determines if the given tile bounds are intersecting each other. */
	static bool CheckCollision(const FTileBound& A, const FTileBound& B);

private:

	/** Determines if the given tile bounds are too far to collide. */
	static bool AreBoundsDistant(const FTileBound& A, const FTileBound& B);

	/** Determines if the given axis is a separating axis for the provided tile bounds. */
	static bool IsAxisSeparating(const FTileBound& A, const FTileBound& B, const FVector& Axis);

	/** Projects the tile bound on the given axis as an interval. */
	FFloatInterval LineProjection(const FVector& Axis) const;
};
