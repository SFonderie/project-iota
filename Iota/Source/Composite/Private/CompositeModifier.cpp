#include "CompositeModifier.h"
#include "CompositeValue.h"

// Define to avoid constantly resetting the default source.
const FName FCompositeModifier::DefaultSource = TEXT("DirectModifier");

float FCompositeModifier::GetValue() const
{
	if (PointerValue)
	{
		return PointerValue->GetValue();
	}

	return ConstantValue;
}
