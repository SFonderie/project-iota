#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CompositeLibrary.generated.h"

// Forward declarations.
class UCompositeValue;
struct FCompositeModifier;

// Library of additional functions
UCLASS()
class COMPOSITE_API UCompositeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// Casts a Composite Value to a float by extracting its value.
	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, CompactNodeTitle = "->", DisplayName = "To Float (Composite Modifier)"))
	static float Conv_CompositeValueToFloat(UCompositeValue* Composite);
};
