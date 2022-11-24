#pragma once

#include "CoreMinimal.h"
#include "CompositeTransform.generated.h"

// Standard composite transformation types.
UENUM()
enum class ECompositeTransform : uint8
{
	// Skip transformation.
	None,

	// Linear transformation.
	Linear,

	// Square transformation.
	Square,

	// Square root transformation. 
	// Defined for all real numbers via an absolute value.
	SquareRoot,

	// Inverse transformation.
	Inverse,

	// Inverse square transformation.
	InverseSquare,

	// Inverse square root transformation.
	// Defined for all real numbers except 0 via an absolute value.
	InverseSquareRoot,

	// Exponential transformation.
	Exponential,

	// Logarithmic transformation.
	// Defined for all real numbers except 0 via an absolute value.
	Logarithmic,

	// Sine transformation.
	Sine,

	// Cosine transformation.
	Cosine,

	// Sigmoid transformation.
	// Calculates using the inverse absolute sigmoid curve.
	Sigmoid,
};

// Applies a transformation to a Composite Value before applying modifiers.
USTRUCT(BlueprintType)
struct COMPOSITE_API FCompositeTransform
{
	GENERATED_BODY()

public:

	// Determines the transformation function D in [ k D ( at + ab ) + C ]
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECompositeTransform Type = ECompositeTransform::None;

	// Value of the leading coefficient k in [ k D ( at + ab ) + C ]
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Leading = 1;

	// Value of the scale coefficient a in [ k D ( at + ab ) + C ]
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Scale = 1;

	// Value of the shift coefficient b in [ k D ( at - ab ) + C ]
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Shift = 0;

	// Value of the constant coefficient C in [ k D ( at + ab ) + C ]
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Constant = 0;

public:

	// Evaluates the transform at the given value.
	float Evaluate(float Value) const;
};
