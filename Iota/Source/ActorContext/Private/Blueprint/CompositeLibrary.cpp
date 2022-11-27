#include "CompositeLibrary.h"


//
// GENERAL FUNCTIONS
//

float UCompositeLibrary::EvaluateBounds(const FCompositeBounds& Bounds, float Value)
{
	return Bounds.Clamp(Value);
}

float UCompositeLibrary::EvaluateTransform(const FCompositeTransform& Transform, float Value)
{
	return Transform.Evaluate(Value);
}


//
// IMPLICIT FUNCTIONS
//

float UCompositeLibrary::Conv_CompositeModifierToFloat(const FCompositeModifier& Modifier)
{
	return Modifier.GetValue();
}

float UCompositeLibrary::Conv_CompositeValueToFloat(UCompositeValue* Composite)
{
	if (Composite)
	{
		return Composite->GetValue();
	}

	return 0;
}
