#pragma once

#include "CoreMinimal.h"
#include "CompositeModifier.generated.h"

// Forward declarations
class UCompositeValue;

// Indicates how a Composite Modifier should apply to a Composite Value.
UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = true))
enum class ECompositeModifier : uint8
{
	// Default flag used for comparisons.
	None = 0x00 UMETA(Hidden),

	// Adds the modifier to a running percent sum. Subsequent percent addition modifiers will
	// continue to add to the sum until a modifier of a different type is encountered, at which
	// point the sum will be factored into the actual total as a multiplier.
	PercentAddition = 0x01,

	// Adds the modifier to a running percent sum. Subsequent inverse addition modifiers will
	// continue to add to the sum until a modifier of a different type is encountered, at which
	// point the sum will be factored into the actual total as a divisor.
	InverseAddition = 0x02,

	// Factors the modifier into the current total as a multiplier.
	Multiplication = 0x04,

	// Factors the modifier into the current total as a divisor.
	Division = 0x08,

	// Adds the modifier to the current total.
	DirectAddition = 0x10,
};

ENUM_CLASS_FLAGS(ECompositeModifier)

// Represents the data needed to modify a Composite Value.
USTRUCT(BlueprintType)
struct COMPOSITE_API FCompositeModifier
{
	GENERATED_BODY()

public:

	// Default modifier source.
	static const FName DefaultSource;

	// Controls how the modifier applies to a composite.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECompositeModifier Type = ECompositeModifier::PercentAddition;

	// Constant modifier value. Used if the pointer value is null.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Value", Tooltip = "Modifier value."))
	float ConstantValue = 0;

	// Pointer to another composite from which to derive a value.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCompositeValue* PointerValue = nullptr;

	// Modifier sort order. Lower values apply earlier.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Order = 0;

	// Name of the object responsible for applying the modifier.
	// Used as a predicate when removing modifiers from a composite.
	UPROPERTY(BlueprintReadOnly)
	FName Source = DefaultSource;

public:

	// Returns the modifier value.
	float GetValue() const;
};
