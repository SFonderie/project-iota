#pragma once

#include "CoreMinimal.h"
#include "CompositeTransform.generated.h"


/// Indicates which function a Composite Transform should use.
UENUM(meta = (ScriptName = "CompositeTransformFunction"))
enum class ECompositeTransform : uint8
{
	/// Does not apply a transformation.
	None,

	/// Applies a linear transformation.
	Linear,

	/// Applies a square transformation.
	Square,

	/// Applies a square-root transformation.
	/// Supports negative inputs via the absolute value.
	SquareRoot,

	/// Applies an inverse transformation.
	Inverse,

	/// Applies an inverse-square transformation.
	InverseSquare,

	/// Applies an inverse-square-root transformation.
	/// Supports negative inputs via the absolute value.
	InverseSquareRoot,

	/// Applies an exponential transformation.
	Exponential,

	/// Applies a logarithmic transformation.
	/// Supports negative inputs via the absolute value.
	Logarithmic,

	/// Applies a sine-wave transformation.
	Sine,

	/// Applies a cosine-wave transformation.
	Cosine,

	/// Applies a sigmoid transformation.
	/// Uses the simple algebraic sigmoid for calculations.
	Sigmoid,
};


/// Represents a parameteric transform function for use with a Composite Value.
/// Normally applies to the composite's base value before any modifiers are applied.
/// Composite Transforms are always functions of the form [ k D ( at - ab ) + C ]
USTRUCT(BlueprintType)
struct COMPOSITE_API FCompositeTransform
{
	GENERATED_BODY()

public:

	/// Determines the transform function D in [ k D ( at + ab ) + C ]
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECompositeTransform Function = ECompositeTransform::None;

	/// Value of the leading coefficient k in [ k D ( at + ab ) + C ]
	/// Stretches or compresses the transform range by a factor equal to itself.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Leading = 1;

	/// Value of the scale coefficient a in [ k D ( at + ab ) + C ]
	/// Compresses or stretches the transform domain by a factor equal to itself.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Scale = 1;

	/// Value of the shift coefficient b in [ k D ( at - ab ) + C ]
	/// Shifts the transform domain right or left by an amount equal to itself.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Shift = 0;

	/// Value of the constant coefficient C in [ k D ( at + ab ) + C ]
	/// Shifts the transform range up or down by an amount equal to itself.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Constant = 0;

public:

	/// Default constructor. Defines a blank function with default parameters.
	FCompositeTransform();

	/// Scalar constructor. Defines a function with only multiplicative parameters.
	/// @param Function Transform function to use.
	/// @param Leading Leading coefficient; stretches or compresses the transform range by a factor equal to itself.
	/// @param Scale Scale coefficient; compresses or stretches the transform domain by a factor equal to itself.
	FCompositeTransform(ECompositeTransform Function, float Leading, float Scale);

	/// Complete constructor. Defines a function with all parameters.
	/// @param Function Transform function to use.
	/// @param Leading Leading coefficient; stretches or compresses the transform range by a factor equal to itself.
	/// @param Scale Scale coefficient; compresses or stretches the transform domain by a factor equal to itself.
	/// @param Shift Shift coefficient; shifts the transform domain right or left by an amount equal to itself.
	/// @param Constant Constant coefficient; shifts the transform range up or down by an amount equal to itself.
	FCompositeTransform(ECompositeTransform Function, float Leading, float Scale, float Shift, float Constant);

	/// Evaluates the transform function at the given value.
	float Evaluate(float Value) const;
};
