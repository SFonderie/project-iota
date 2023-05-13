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

	/** Bounding box center in parent space. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Center;

	/** Bounding box orientation. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator Rotation;

	/** Bounding box extent in local space. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Extent;
};
