#include "CompositeLibrary.h"
#include "CompositeValue.h"

float UCompositeLibrary::Conv_CompositeValueToFloat(UCompositeValue* Composite)
{
	return Composite->GetValue();
}
