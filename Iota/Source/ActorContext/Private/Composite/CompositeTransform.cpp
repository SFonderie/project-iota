#include "Composite/CompositeTransform.h"


FCompositeTransform::FCompositeTransform()
{
	// Default constructor.
}

FCompositeTransform::FCompositeTransform(ECompositeTransform Function, float Leading, float Scale) :
	Function(Function), Leading(Leading), Scale(Scale)
{
	// Scalar constructor.
}

FCompositeTransform::FCompositeTransform(ECompositeTransform Function, float Leading, float Scale, float Shift, float Constant) :
	Function(Function), Leading(Leading), Scale(Scale), Shift(Shift), Constant(Constant)
{
	// Complete constructor.
}

float FCompositeTransform::Evaluate(float Value) const
{
	switch (Function)
	{
		case ECompositeTransform::Linear:
		{
			return Leading * (Scale * (Value - Shift)) + Constant;
		}
		case ECompositeTransform::Square:
		{
			return Leading * FMath::Square(Scale * (Value - Shift)) + Constant;
		}
		case ECompositeTransform::SquareRoot:
		{
			float Absolute = FMath::Abs(Scale * (Value - Shift));
			return Leading * FMath::Sqrt(Absolute) + Constant;
		}
		case ECompositeTransform::Inverse:
		{
			return Leading / (Scale * (Value - Shift)) + Constant;
		}
		case ECompositeTransform::InverseSquare:
		{
			return Leading / FMath::Square(Scale * (Value - Shift)) + Constant;
		}
		case ECompositeTransform::InverseSquareRoot:
		{
			float Absolute = FMath::Abs(Scale * (Value - Shift));
			return Leading * FMath::InvSqrt(Absolute) + Constant;
		}
		case ECompositeTransform::Exponential:
		{
			return Leading * FMath::Exp(Scale * (Value - Shift)) + Constant;
		}
		case ECompositeTransform::Logarithmic:
		{
			float Absolute = FMath::Abs(Scale * (Value - Shift));
			return Leading * FMath::Loge(Absolute) + Constant;
		}
		case ECompositeTransform::Sine:
		{
			return Leading * FMath::Sin(Scale * (Value - Shift)) + Constant;
		}
		case ECompositeTransform::Cosine:
		{
			return Leading * FMath::Cos(Scale * (Value - Shift)) + Constant;
		}
		case ECompositeTransform::Sigmoid:
		{
			float Inner = Scale * (Value - Shift);
			return Leading * (Inner / (FMath::Abs(Inner) + 1)) + Constant;
		}
		default:
		{
			return Value;
		}
	}
}
