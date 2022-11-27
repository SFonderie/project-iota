#include "Composite/CompositeModifier.h"
#include "Composite/CompositeValue.h"


// Define static FNames to avoid expensive redeclaration calls.
const FName FCompositeModifier::DefaultSource = TEXT("DirectModifier");
const FName FCompositeModifier::IntrinsicSource = TEXT("IntrinsicModifier");


FCompositeModifier::FCompositeModifier()
{
	// Default constructor.
}

FCompositeModifier::FCompositeModifier(ECompositeModifier Type, float Constant, int32 Order, const FName& Source) :
	Type(Type), ConstantValue(Constant), Order(Order), Source(Source)
{
	// Constant constructor.
}

FCompositeModifier::FCompositeModifier(ECompositeModifier Type, UCompositeValue* Pointer, int32 Order) :
	Type(Type), PointerValue(Pointer), Order(Order), Source(IntrinsicSource)
{
	// Pointer constructor.
}

float FCompositeModifier::GetValue() const
{
	if (PointerValue)
	{
		return PointerValue->GetValue();
	}

	return ConstantValue;
}
