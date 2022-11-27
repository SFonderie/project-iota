#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Composite/CompositeBounds.h"
#include "Composite/CompositeModifier.h"
#include "Composite/CompositeTransform.h"
#include "Composite/CompositeValue.h"
#include "CompositeLibrary.generated.h"


/// Provides additional composite behaviors for use in the editor.
UCLASS()
class ACTORCONTEXT_API UCompositeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


	//
	// GENERAL FUNCTIONS
	//

	/// Evaluates the bounds at the given value.
	UFUNCTION(BlueprintPure, Category = "Composite")
	static float EvaluateBounds(const FCompositeBounds& Bounds, float Value);

	/// Evaluates the transform function at the given value.
	UFUNCTION(BlueprintPure, Category = "Composite")
	static float EvaluateTransform(const FCompositeTransform& Transform, float Value);


	//
	// IMPLICIT FUNCTIONS
	//

	/// Converts a Composite Modifier to a float by extracting its value.
	UFUNCTION(BlueprintPure, Category = "Composite", meta = (BlueprintAutocast, CompactNodeTitle = "->", DisplayName = "To Float (Composite Modifier)"))
	static float Conv_CompositeModifierToFloat(const FCompositeModifier& Modifier);

	/// Converts a Composite Value to a float by extracting its calculated value.
	UFUNCTION(BlueprintPure, Category = "Composite", meta = (BlueprintAutocast, CompactNodeTitle = "->", DisplayName = "To Float (Composite Value)"))
	static float Conv_CompositeValueToFloat(UCompositeValue* Composite);
};
