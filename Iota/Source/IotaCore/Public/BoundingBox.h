// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BoundingBox.generated.h"

/** Simple implementation of an oriented bounding box. */
USTRUCT(BlueprintType)
struct IOTACORE_API FBoundingBox
{
	GENERATED_BODY()

public:

	/** Bounding box center in world space. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Center;

	/** Bounding box orientation in world space. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator Rotation;

	/** Bounding box extent (half-size) in local space. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Extent;

	/** Default constructor; creates an axis-aligned unit box at the origin. */
	FBoundingBox();

	/** Complete constructor; creates an oriented box at the given location. */
	FBoundingBox(const FVector& InCenter, const FRotator& InRotation, const FVector& InExtent);

	/** Copy constructor; duplicates the given bounding box and optionally transforms it. */
	FBoundingBox(const FBoundingBox& SourceBox, const FTransform& Transform = FTransform::Identity);

	/** Returns the bounding box forward vector in world space. */
	FVector GetAxisX() const
	{
		return Rotation.RotateVector(FVector::ForwardVector);
	}

	/** Returns the bounding box right vector in world space. */
	FVector GetAxisY() const
	{
		return Rotation.RotateVector(FVector::RightVector);
	}

	/** Returns the bounding box up vector in world space. */
	FVector GetAxisZ() const
	{
		return Rotation.RotateVector(FVector::UpVector);
	}

	/** Performs a collision check against the given box. */
	bool IsCollidingWith(const FBoundingBox& Collider) const
	{
		return FBoundingBox::CheckSeparatingAxes(*this, Collider);
	}

private:

	/** Performs a collision check between two oriented bounding boxes via the Separating Axis Theorem. */
	static bool CheckSeparatingAxes(const FBoundingBox& BoxA, const FBoundingBox& BoxB);

	/**
	 * Projects two bounding boxes along a test axis and checks for an overlap. If the two intervals
	 * do not overlap, then the axis is a separating axis and the method will return true.
	 */
	static bool TestSeparatingAxis(const FBoundingBox& BoxA, const FBoundingBox& BoxB, const FVector& Axis);

	/** Projects the bounding box along the given axis and returns the result as a float interval. */
	FFloatInterval MakeLineProjection(const FVector& Axis) const;

	/** Fills the given array with the three box axes. */
	void LoadAxesBuffer(TArray<FVector>& OutAxesBuffer) const
	{
		OutAxesBuffer.Empty(3);
		OutAxesBuffer.Emplace(GetAxisX());
		OutAxesBuffer.Emplace(GetAxisY());
		OutAxesBuffer.Emplace(GetAxisZ());
	}
};
