// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "IotaMathLibrary.h"
#include "BoundingBox.h"

void UIotaMathLibrary::GetBoundingBoxAxes(const FBoundingBox& Box, FVector& AxisX, FVector& AxisY, FVector& AxisZ)
{
	AxisX = Box.GetAxisX();
	AxisY = Box.GetAxisY();
	AxisZ = Box.GetAxisZ();
}

FBoundingBox UIotaMathLibrary::FromBoxComponent(const UBoxComponent* BoxComponent)
{
	return FBoundingBox(BoxComponent);
}

FBoundingBox UIotaMathLibrary::TransformBoundingBox(const FBoundingBox& SourceBox, const FTransform& Transform)
{
	return FBoundingBox(SourceBox, Transform);
}

bool UIotaMathLibrary::CheckBoxCollision(const FBoundingBox& BoxA, const FBoundingBox& BoxB)
{
	return BoxA.IsCollidingWith(BoxB);
}
