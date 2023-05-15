// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "BoundingBox.h"
#include "Components/BoxComponent.h"

FBoundingBox::FBoundingBox()
	: Extent(FVector::OneVector * 50)
{
	// Default constructor.
}

FBoundingBox::FBoundingBox(const FVector& InCenter, const FRotator& InRotation, const FVector& InExtent)
	: Center(InCenter)
	, Rotation(InRotation)
	, Extent(InExtent)
{
	// Complete constructor.
}

FBoundingBox::FBoundingBox(const FBoundingBox& SourceBox, const FTransform& Transform)
	: Center(Transform.TransformPosition(SourceBox.Center))
	, Rotation(Transform.TransformRotation(SourceBox.Rotation.Quaternion()).Rotator())
	, Extent(Transform.GetScale3D()* SourceBox.Extent)
{
	// Copy constructor.
}

FBoundingBox::FBoundingBox(const UBoxComponent* BoxComponent)
	: Center(BoxComponent->GetComponentLocation())
	, Rotation(BoxComponent->GetComponentRotation())
	, Extent(BoxComponent->GetScaledBoxExtent())
{
	// Box constructor.
}

bool FBoundingBox::CheckSeparatingAxes(const FBoundingBox& BoxA, const FBoundingBox& BoxB)
{
	TArray<FVector> AxesBufferA, AxesBufferB;

	// Load the coordinate axes.
	BoxA.LoadAxesBuffer(AxesBufferA);
	BoxB.LoadAxesBuffer(AxesBufferB);

	// Box A - Test all face normals.
	for (const FVector& Axis : AxesBufferA)
	{
		if (TestSeparatingAxis(BoxA, BoxB, Axis))
		{
			return false;
		}
	}

	// Box B - Test all face normals.
	for (const FVector& Axis : AxesBufferB)
	{
		if (TestSeparatingAxis(BoxA, BoxB, Axis))
		{
			return false;
		}
	}

	// Test all edge normals (cross products).
	for (const FVector& AxisA : AxesBufferA)
	{
		for (const FVector& AxisB : AxesBufferB)
		{
			if (TestSeparatingAxis(BoxA, BoxB, AxisA ^ AxisB))
			{
				return false;
			}
		}
	}

	return true;
}

bool FBoundingBox::TestSeparatingAxis(const FBoundingBox& BoxA, const FBoundingBox& BoxB, const FVector& Axis)
{
	if (Axis.IsNearlyZero())
	{
		return false;
	}

	FFloatInterval IntervalA = BoxA.MakeLineProjection(Axis);
	FFloatInterval IntervalB = BoxB.MakeLineProjection(Axis);

	// If either bound of B is inside of A, then the two intervals must overlap and
	// therefore the axis is not a valid separating axis.
	return !IntervalA.Contains(IntervalB.Min) && !IntervalA.Contains(IntervalB.Max);
}

FFloatInterval FBoundingBox::MakeLineProjection(const FVector& Axis) const
{
	// Projection becomes much cleaner when the target vector is normalized.
	const FVector UnitAxis = Axis.IsNormalized() ? Axis : Axis.GetUnsafeNormal();
	const float Signs[] = { -1.0f, 1.0f };

	// Calculate the distance of the box vertices from the line using normal projection.
	// Since the dot product is distributive ( A * (B + C) = A * B + A * C ), we can now
	// pre-calulate the extent offsets and then add them together as floats later.
	float CenterDistance = UnitAxis | Center;
	float XOffset = UnitAxis | (GetAxisX() * Extent.X);
	float YOffset = UnitAxis | (GetAxisY() * Extent.Y);
	float ZOffset = UnitAxis | (GetAxisZ() * Extent.Z);

	// Eventual output value.
	FFloatInterval LineProjection;

	// Iterate through each vertex.
	for (int32 i = 0; i < 2; i++)
	{
		for (int32 j = 0; j < 2; j++)
		{
			for (int32 k = 0; k < 2; k++)
			{
				// Calculate the total distance of the projected vertex from the arbitrary line origin by adding the
				// half-size projection offsets to the center distance.
				float VertexDistance = CenterDistance + Signs[i] * XOffset + Signs[j] * YOffset + Signs[k] * ZOffset;

				// Include the distance in the interval.
				LineProjection.Include(VertexDistance);
			}
		}
	}

	return LineProjection;
}
