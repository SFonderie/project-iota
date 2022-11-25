#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Composite.h"
#include "CompositeLibrary.generated.h"


/// Provides additional composite behaviors for use in the editor.
UCLASS()
class COMPOSITE_API UCompositeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/// Converts a Composite Value to a float by extracting its calculated value.
	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, CompactNodeTitle = "->", DisplayName = "To Float (Composite Value)"))
	static float Conv_CompositeValueToFloat(UCompositeValue* Composite);
};
