#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CompositeTransform.h"
#include "CompositeModifier.h"
#include "CompositeValue.generated.h"

// Controls how a Composite Value clamps itself.
UENUM()
enum class ECompositeBounds : uint8
{
	// Value has no bounds.
	None,

	// Ensures the value will always exceed a minimum.
	// Clamps to the lower bound.
	Minimum,

	// Ensures the value will never exceed a maximum.
	// Clamps to the upper bound.
	Maximum,

	// Ensures the value always falls on the bound interval.
	// Clamps to the nearest bound.
	Interval,

	// Excludes the value from the bound interval.
	// Clamps to the upper bound.
	Exclusion,
};

// Mutable statistic composed of a base value and modifiers.
UCLASS(BlueprintType)
class COMPOSITE_API UCompositeValue : public UObject
{
	GENERATED_BODY()

private:

	// Set of all composites dependent on this composite's final value.
	// When this composite is marked dirty, so are all of its listeners.
	UPROPERTY()
	TMap<UCompositeValue*, int32> Listeners;

	// Constant base value. Used if the pointer base is null.
	UPROPERTY(BlueprintGetter = GetConstantBase, BlueprintSetter = SetConstantBase)
	float ConstantBase = 0;

	// Pointer to another composite from which to derive a base value.
	UPROPERTY(BlueprintGetter = GetPointerBase, BlueprintSetter = SetPointerBase)
	UCompositeValue* PointerBase = nullptr;

	// Transforms the base value before applying modifiers.
	UPROPERTY(BlueprintGetter = GetTransform, BlueprintSetter = SetTransform)
	FCompositeTransform TransformFunction;

	// Prevents masked modifier types from applying to the composite.
	UPROPERTY()
	ECompositeModifier ModifierMask = ECompositeModifier::None;

	// Array of all modifiers currently applied to this composite.
	UPROPERTY()
	TArray<FCompositeModifier> Modifiers;

	// Ensures that the composite is recalculated after a state change.
	UPROPERTY()
	bool IsDirty = false;

	// Controls how the composite value is clamped to its bounds.
	UPROPERTY(BlueprintGetter = GetBoundsType, BlueprintSetter = SetBoundsType)
	ECompositeBounds BoundsType = ECompositeBounds::None;

	// Composite lower bound. Used according to the bounds type.
	UPROPERTY(BlueprintGetter = GetLowerBound, BlueprintSetter = SetLowerBound)
	float LowerBound = 0;

	// Composite upper bound. Used according to the bounds type.
	UPROPERTY(BlueprintGetter = GetUpperBound, BlueprintSetter = SetUpperBound)
	float UpperBound = 0;

	// Actual composite value.
	UPROPERTY(BlueprintGetter = GetValue)
	float Value = 0;

public:

	// Default constructor.
	UCompositeValue();

private:

	//
	// LISTENERS
	//

	// Adds the given listener to the composite set.
	// If the listener is already in the set, increment it instead.
	void AddListener(UCompositeValue* Listener);

	// Decrements the given listener in the composite set.
	// If the listener would decrement to zero, remove it instead.
	void RemoveListener(UCompositeValue* Listener);

public:

	//
	// BASE VALUES
	//

	// Returns the constant base value of the composite.
	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = true))
	float GetConstantBase() const;

	// Returns the pointer base value of the composite.
	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = true))
	UCompositeValue* GetPointerBase() const;

	// Sets the constant base value of the composite.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true))
	void SetConstantBase(float Constant);

	// Sets the pointer base value of the composite.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true))
	void SetPointerBase(UCompositeValue* Pointer);

	// Returns the current base value of the composite.
	UFUNCTION(BlueprintPure)
	float GetBaseValue() const;

	// Replaces the composite's current base value with the given constant.
	UFUNCTION(BlueprintCallable)
	void MakeBaseConstant(float Constant);

	// Replaces the composite's current base value with the given pointer.
	UFUNCTION(BlueprintCallable)
	void MakeBasePointer(UCompositeValue* Pointer);

public:

	//
	// TRANSFORM
	//

	// Returns the composite's transform function.
	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = true))
	const FCompositeTransform& GetTransform() const;

	// Sets the composite's transform function.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true))
	void SetTransform(const FCompositeTransform& Transform);

private:

	//
	// MODIFIERS
	//

	// Removes a listener from the modifier's pointer value if the given condition is true.
	// Used during RemoveAll predicates to delink the modifier and simplify the lambda.
	bool RemoverPredicate(const FCompositeModifier& Modifier, bool PredicateResult);

public:

	// Define a mask preventing certain modifier types from applying to the composite.
	UFUNCTION(BlueprintCallable)
	void DefineMask(UPARAM(meta = (Bitmask, BitmaskEnum = ECompositeModifier)) int32 Mask);

	// Tries to add the given modifier to the composite.
	// Returns true if the modifier was successfully added.
	UFUNCTION(BlueprintCallable)
	bool AddModifier(const FCompositeModifier& Modifier);

	// Removes modifiers with the given source from the composite.
	// Returns true if any modifiers were successfully removed.
	UFUNCTION(BlueprintCallable)
	bool RemoveModifiers(const FName& Source);

	// Removes all modifiers from the composite.
	UFUNCTION(BlueprintCallable)
	void ClearModifiers();

private:

	//
	// PARAMETERS
	//

	// Marks the composite for recalculation.
	void MarkDirty();

public:

	// Returns the bounds type of the composite.
	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = true))
	ECompositeBounds GetBoundsType() const;

	// Returns the lower bound of the composite.
	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = true))
	float GetLowerBound() const;

	// Returns the upper bound of the composite.
	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = true))
	float GetUpperBound() const;

	// Sets the bounds type of the composite.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true))
	void SetBoundsType(ECompositeBounds Bounds);

	// Sets the lower bound of the composite.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true))
	void SetLowerBound(float Lower);

	// Sets the upper bound of the composite.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true))
	void SetUpperBound(float Upper);

	// Defines the composite bounds type and their values.
	UFUNCTION(BlueprintCallable)
	void DefineBounds(ECompositeBounds Bounds = ECompositeBounds::None, float Lower = 0, float Upper = 0);

private:

	//
	// CALCULATION
	//

	// Actually calculates the composite value.
	void Calculate();

public:

	// Returns the composite value.
	UFUNCTION(BlueprintPure)
	float GetValue();
};
