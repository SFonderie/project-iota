#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CompositeTransform.h"
#include "CompositeModifier.h"
#include "CompositeBounds.h"
#include "CompositeValue.generated.h"


/// Mutable statistic composed of a base value and modifiers.
UCLASS(BlueprintType)
class COMPOSITE_API UCompositeValue : public UObject
{
	GENERATED_BODY()

private:

	/// Set of all composites dependent on this composite's calculated value.
	/// When a composite is marked for recalculation, so are all of its listeners.
	UPROPERTY()
	TMap<UCompositeValue*, uint8> Listeners;

	/// Constant base value. Used if the pointer base is null.
	UPROPERTY(BlueprintGetter = GetConstantBase, BlueprintSetter = SetConstantBase)
	float ConstantBase = 0;

	/// Pointer to another composite from which to derive a base value.
	UPROPERTY(BlueprintGetter = GetPointerBase, BlueprintSetter = SetPointerBase)
	UCompositeValue* PointerBase = nullptr;

	/// Transforms the base value before applying modifiers.
	UPROPERTY(BlueprintGetter = GetTransform, BlueprintSetter = SetTransform)
	FCompositeTransform Transform;

	/// Prevents certain modifier types from applying to the composite.
	UPROPERTY()
	ECompositeModifier ModifierMask = ECompositeModifier::None;

	/// List of all modifiers currently applied to this composite.
	UPROPERTY()
	TArray<FCompositeModifier> Modifiers;

	/// Provides optional constraints for the calculated value.
	UPROPERTY(BlueprintGetter = GetBounds, BlueprintSetter = SetBounds)
	FCompositeBounds Bounds;

	/// Ensures that the composite is calculated after a state change.
	UPROPERTY()
	bool bDirty = false;

	/// The actual composite value.
	UPROPERTY(BlueprintGetter = GetValue)
	float Value = 0;

public:

	/// Default constructor.
	UCompositeValue();


private:

	//
	// LISTENERS
	//

	/// Adds the given listener to the set. If the listener is already in the set, increment its
	/// associated value instead.
	void AddListener(UCompositeValue* Listener);

	/// Decrement the given listener's value in the set. If doing so would reduce its value to
	/// zero, then remove it from the set instead.
	void RemoveListener(UCompositeValue* Listener);

public:

	/// Returns a read-only reference to the listeners set.
	const TMap<UCompositeValue*, uint8>& GetListeners() const;


public:

	//
	// BASE VALUES
	//

	/// Returns the constant base value of the composite.
	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = true))
	float GetConstantBase() const;

	/// Returns the pointer base value of the composite.
	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = true))
	UCompositeValue* GetPointerBase() const;

	/// Sets the constant base value of the composite.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true))
	void SetConstantBase(float Constant);

	/// Sets the pointer base value of the composite.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true))
	void SetPointerBase(UCompositeValue* Pointer);

	/// Returns the current base value of the composite.
	UFUNCTION(BlueprintPure)
	float GetBaseValue() const;

	/// Replaces the composite's current base value with the given constant.
	UFUNCTION(BlueprintCallable)
	void MakeBaseConstant(float Constant);

	/// Replaces the composite's current base value with the given pointer.
	UFUNCTION(BlueprintCallable)
	void MakeBasePointer(UCompositeValue* Pointer);


private:

	//
	// TRANSFORMS
	//

	/// Defines a transform function for the composite. Blueprint use only.
	/// @param Function Breakable transform structure.
	UFUNCTION(BlueprintCallable)
	void DefineTransform(const FCompositeTransform& Function);

public:

	/// Returns the composite's transform function.
	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = true))
	const FCompositeTransform& GetTransform() const;

	/// Sets the composite's transform function.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true))
	void SetTransform(const FCompositeTransform& Function);


private:

	//
	// MODIFIERS
	//

	/// Removes a listener from the modifier's pointer value if the given condition is true.
	/// Used during RemoveAll predicates to delink the modifier and simplify the lambda.
	/// @param Modifier Element being potentially delinked.
	/// @param bPredicate Whether the element will be removed.
	bool DelinkerPredicate(const FCompositeModifier& Modifier, bool bPredicate);

public:

	/// Returns a read-only reference to the modifiers list.
	const TArray<FCompositeModifier>& GetModifiers() const;

	/// Define a mask preventing certain modifier types from applying to the composite.
	UFUNCTION(BlueprintCallable)
	void DefineMask(UPARAM(meta = (Bitmask, BitmaskEnum = ECompositeModifier)) int32 Mask);

	/// Attempts to add the given modifier to the composite.
	/// Returns true if the modifier was successfully added.
	UFUNCTION(BlueprintCallable)
	bool AddModifier(const FCompositeModifier& Modifier);

	/// Removes modifiers with the given source from the composite.
	/// Returns true if any modifiers were successfully removed.
	UFUNCTION(BlueprintCallable)
	bool RemoveModifiers(const FName& Source);

	/// Removes all non-intrinsic modifiers from the composite.
	UFUNCTION(BlueprintCallable)
	void ClearModifiers();


private:

	//
	// BOUNDS
	//

	/// Defines a set of bounds for the composite. Blueprint use only.
	/// @param Function Breakable bounds structure.
	UFUNCTION(BlueprintCallable)
	void DefineBounds(const FCompositeBounds& Limit);

public:

	/// Returns the composite bounds.
	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = true))
	const FCompositeBounds& GetBounds() const;

	/// Sets the composite bounds.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true))
	void SetBounds(const FCompositeBounds& Limit);


private:

	//
	// CALCULATION
	//

	/// Marks the composite for recalculation.
	void MarkDirty();

	/// Actually calculates the composite value.
	void Calculate();

public:

	/// Returns the composite value.
	UFUNCTION(BlueprintPure)
	float GetValue();
};
