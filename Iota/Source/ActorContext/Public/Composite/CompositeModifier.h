#pragma once

#include "CoreMinimal.h"
#include "CompositeModifier.generated.h"


// Forward declarations
class UCompositeValue;


/// Indicates how a Composite Modifier should apply to a Composite Value.
UENUM(meta = (Bitflags, ScriptName = "CompositeModifierType", UseEnumValuesAsMaskValuesInEditor = true))
enum class ECompositeModifier : uint8
{
	/// Default flag used for comparisons.
	None = 0x00 UMETA(Hidden),

	/// Adds the modifier to a running percent sum. Subsequent percent addition modifiers will
	/// continue to add to the sum until a modifier of a different type is encountered, at which
	/// point the sum will be factored into the actual total as a multiplier.
	PercentAddition = 0x01,

	/// Adds the modifier to a running percent sum. Subsequent inverse addition modifiers will
	/// continue to add to the sum until a modifier of a different type is encountered, at which
	/// point the sum will be factored into the actual total as a divisor.
	InverseAddition = 0x02,

	/// Factors the modifier into the current total as a multiplier.
	Multiplication = 0x04,

	/// Factors the modifier into the current total as a divisor.
	Division = 0x08,

	/// Adds the modifier to the current total.
	DirectAddition = 0x10,
};


ENUM_CLASS_FLAGS(ECompositeModifier)


/// Modifies the content of a Composite Value.
USTRUCT(BlueprintType)
struct ACTORCONTEXT_API FCompositeModifier
{
	GENERATED_BODY()

public:

	/// Default modifier source name. Used as the default argument for removal methods.
	static const FName DefaultSource;

	/// Marks the modifier as intrinsic, allowing it to persist across resets. When a Composite
	/// Value invokes the ClearModifiers method, the predicate removes all modifiers that do 
	/// not possess this source. Most pointer modifiers are intrinsic.
	static const FName IntrinsicSource;

	/// Determines how the modifier applies to a composite.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECompositeModifier Type = ECompositeModifier::PercentAddition;

	/// Constant modifier value. Used if the pointer value is null.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Value", Tooltip = "Actual modifier value."))
	float ConstantValue = 0;

	/// Pointer to another composite from which to derive a value.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCompositeValue* PointerValue = nullptr;

	/// Determines the order in which modifiers apply to a composite.
	/// Lower order values apply earlier.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Order = 0;

	/// Name of the object responsible for applying the modifier.
	/// Used as a predicate when removing modifiers from a composite.
	UPROPERTY(BlueprintReadOnly)
	FName Source = DefaultSource;

public:

	/// Default constructor. Creates an empty modifier.
	FCompositeModifier();

	/// Constant constructor. Creates a normal modifier with an optional source name.
	/// @param Type Determines how the modifier applies to a composite.
	/// @param Constant Constant modifier value.
	/// @param Order Determines the order in which modifiers apply to a composite.
	/// @param Source Optional source name.
	FCompositeModifier(ECompositeModifier Type, float Constant, int32 Order, const FName& Source = DefaultSource);

	/// Pointer constructor. Creates an intrinsic modifier that points to the given composite.
	/// @param Type Determines how the modifier applies to a composite.
	/// @param Pointer Pointer to another composite from which to derive a value.
	/// @param Order Determines the order in which modifiers apply to a composite.
	FCompositeModifier(ECompositeModifier Type, UCompositeValue* Pointer, int32 Order);

	/// Returns the modifier value. 
	float GetValue() const;
};
