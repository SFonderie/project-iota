#pragma once

#include "CoreMinimal.h"
#include "CompositeBounds.generated.h"


/// Indicates how a Composite Bounds should clamp values.
UENUM(meta = (ScriptName = "CompositeBoundsType"))
enum class ECompositeBounds : uint8
{
	/// Do not apply any bounds.
	None,

	/// Applies an inclusive minimum using the lower bound.
	/// Rounds values below the lower bound to the lower bound.
	Minimum,

	/// Applies an inclusive maximum using the upper bound.
	/// Rounds values above the upper bound to the upper bound.
	Maximum,

	/// Limits values to an inclusive interval defined by the bounds.
	/// Rounds values outside the interval to the nearest bound.
	Interval,

	/// Limits values to an exclusive interval defined by the bounds.
	/// Rounds values inside the interval to the nearest bound.
	Exclusion,
};


/// Restricts the calculated output of a Composite Value.
USTRUCT(BlueprintType)
struct COMPOSITE_API FCompositeBounds
{
	GENERATED_BODY()

public:

	/// Determines which set of bounds applies to the composite.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECompositeBounds Bounds = ECompositeBounds::None;

	/// Lower bounding value. Use depends on the bounding type.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Lower = 0;

	/// Upper bounding value. Use depends on the bounding type.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Upper = 0;

public:

	/// Default constructor. Defines an empty bounds object.
	FCompositeBounds();

	/// Complete constructor.
	/// @param Bounds Determines which set of bounds applies to the composite.
	/// @param Lower Lower bounding value. Use depends on the bounding type.
	/// @param Upper Upper bounding value. Use depends on the bounding type.
	FCompositeBounds(ECompositeBounds Bounds, float Lower, float Upper);

	/// Clamps the given value to the bounds.
	float Clamp(float Value) const;
};
