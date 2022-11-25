#include "CompositeBounds.h"


FCompositeBounds::FCompositeBounds()
{
	// Default constructor.
}

FCompositeBounds::FCompositeBounds(ECompositeBounds Bounds, float Lower, float Upper) :
	Bounds(Bounds), Lower(Lower), Upper(Upper)
{
	// Complete constructor.
}

float FCompositeBounds::Clamp(float Value) const
{
	switch (Bounds)
	{
		case ECompositeBounds::Minimum:
		{
			return Value < Lower ? Lower : Value;
		}
		case ECompositeBounds::Maximum:
		{
			return Value > Upper ? Upper : Value;
		}
		case ECompositeBounds::Interval:
		{
			return FMath::Clamp(Value, Lower, Upper);
		}
		case ECompositeBounds::Exclusion:
		{
			if (Value > Lower && Value < Upper)
			{
				return (Value - Lower) < (Upper - Value) ? Lower : Upper;
			}
		}
		default:
		{
			return Value;
		}
	}
}
