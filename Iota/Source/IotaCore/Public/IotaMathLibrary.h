// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IotaMathLibrary.generated.h"

// Forward declarations.
struct FBoundingBox;
class UBoxComponent;

/** Provides additional math utilities for IotaCore. */
UCLASS()
class IOTACORE_API UIotaMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Returns the orthonormal basis for the given bounding box.
	 *
	 * @param Box Derive the basis from this bounding box.
	 * @param AxisX The forward axis of the given bounding box.
	 * @param AxisY The right axis of the given bounding box.
	 * @param AxisZ The up axis of the given bounding box.
	 */
	UFUNCTION(BlueprintPure, Category = "Math|BoundingBox")
	static void GetBoundingBoxAxes(const FBoundingBox& Box, FVector& AxisX, FVector& AxisY, FVector& AxisZ);

	/**
	 * Creates a bounding box representing the given component in world space.
	 *
	 * @param BoxComponent Box Component from which to construct a bounding box.
	 * @return The Box Component as a world-space bounding box structure.
	 */
	UFUNCTION(BlueprintPure, Category = "Math|BoundingBox")
	static FBoundingBox FromBoxComponent(const UBoxComponent* BoxComponent);

	/**
	 * Transforms and returns the bounding box into the given coordinate space.
	 *
	 * @param SourceBox Bounding box to transform.
	 * @param Transform Transforms the bounding box.
	 * @return Transformed bounding box.
	 */
	UFUNCTION(BlueprintPure, Category = "Math|BoundingBox")
	static FBoundingBox TransformBoundingBox(const FBoundingBox& SourceBox, const FTransform& Transform);

	/**
	 * Performs a collision check between two oriented bounding boxes.
	 *
	 * @param BoxA The first oriented bounding box to check.
	 * @param BoxB The second oriented bounding box to check.
	 * @return Whether the two boxes are colliding.
	 */
	UFUNCTION(BlueprintCallable, Category = "Math|BoundingBox")
	static bool CheckBoxCollision(const FBoundingBox& BoxA, const FBoundingBox& BoxB);
};
