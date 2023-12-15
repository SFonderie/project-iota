// Copyright Sydney Fonderie. All Rights Reserved.

#include "TileData/TileBound.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileBound)

FTileBound::FTileBound()
	: Center(0, 0, 0)
	, Rotation(0, 0, 0)
	, Extent(50, 50, 50)
{
	// Default constructor.
}

FTileBound::FTileBound(const FVector& InCenter, const FRotator& InRotation, const FVector& InExtent)
	: Center(InCenter)
	, Rotation(InRotation)
	, Extent(InExtent)
{
	// Complete constructor.
}

FTileBound::FTileBound(const FTileBound& TileBound, const FTransform& Transform)
	: Center(Transform.TransformPosition(TileBound.Center))
	, Rotation(Transform.TransformRotation(TileBound.Rotation.Quaternion()).Rotator())
	, Extent(TileBound.Extent)
{
	// Copy constructor.
}

bool FTileBound::CheckCollision(const FTileBound& A, const FTileBound& B)
{
	// Perform a bounding sphere check.
	if (AreBoundsDistant(A, B))
	{
		return false;
	}

	// Load the oriented box axes into array buffers for looping.
	FVector BufferA[] = { A.GetAxisX(), A.GetAxisY(), A.GetAxisZ() };
	FVector BufferB[] = { B.GetAxisX(), B.GetAxisY(), B.GetAxisZ() };

	// Check each face normal on the first box for a separating axis.
	for (const FVector& Axis : BufferA)
	{
		if (IsAxisSeparating(A, B, Axis))
		{
			return false;
		}
	}

	// Check each face normal on the second box for a separating axis.
	for (const FVector& Axis : BufferB)
	{
		if (IsAxisSeparating(A, B, Axis))
		{
			return false;
		}
	}

	// Check each edge vector cross product for a separating axis.
	for (const FVector& AxisA : BufferA)
	{
		for (const FVector& AxisB : BufferB)
		{
			if (IsAxisSeparating(A, B, AxisA ^ AxisB))
			{
				return false;
			}
		}
	}

	// If no separating axis was found, the bounds are colliding.
	return true;
}

bool FTileBound::AreBoundsDistant(const FTileBound& A, const FTileBound& B)
{
	// If the distance between the centers is greater than the sum of the bound 
	// radii, then the bounds are too far away to ever collide with one another.
	return (A.Center - B.Center).Size() > A.Extent.Size() + B.Extent.Size();
}

bool FTileBound::IsAxisSeparating(const FTileBound& A, const FTileBound& B, const FVector& Axis)
{
	FFloatInterval IntervalA = A.LineProjection(Axis);
	FFloatInterval IntervalB = B.LineProjection(Axis);

	// Axis is a separating axis if the projection intervals do not overlap.
	return (IntervalA.Max < IntervalB.Min) || (IntervalB.Max < IntervalA.Min);
}

FFloatInterval FTileBound::LineProjection(const FVector& Axis) const
{
	const float Signs[] = { -1, 1 };
	const float Shrink = 1;

	// Calculate the positions of the box vertices on the line axis using projection.
	// Since the dot-product is distributive ( A * ( B + C ) = A * B + A * C ), we can
	// pre-calculate the extent offsets and add them later.
	float CenterDistance = Axis | Center;
	float XOffset = Axis | (GetAxisX() * (Extent.X - Shrink));
	float YOffset = Axis | (GetAxisY() * (Extent.Y - Shrink));
	float ZOffset = Axis | (GetAxisZ() * (Extent.Z - Shrink));

	FFloatInterval ProjectionInterval;

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
				ProjectionInterval.Include(VertexDistance);
			}
		}
	}

	return ProjectionInterval;
}
