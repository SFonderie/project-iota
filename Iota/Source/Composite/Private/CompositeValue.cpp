#include "CompositeValue.h"

UCompositeValue::UCompositeValue()
{
	// Default constructor.
	// Required to ensure that composites can be created in the editor.
}

//
// LISTENERS
//

void UCompositeValue::AddListener(UCompositeValue* Listener)
{
	int32* Count = Listeners.Find(Listener);

	if (Count)
	{
		(*Count)++;
		return;
	}

	Listeners.Emplace(Listener, 1);
}

void UCompositeValue::RemoveListener(UCompositeValue* Listener)
{
	int32* Count = Listeners.Find(Listener);

	if (Count)
	{
		if (*Count > 1)
		{
			(*Count)--;
			return;
		}

		Listeners.Remove(Listener);
	}
}

//
// BASE VALUES
//

float UCompositeValue::GetConstantBase() const
{
	return ConstantBase;
}

UCompositeValue* UCompositeValue::GetPointerBase() const
{
	return PointerBase;
}

void UCompositeValue::SetConstantBase(float Constant)
{
	ConstantBase = Constant;

	if (!PointerBase)
	{
		MarkDirty();
	}
}

void UCompositeValue::SetPointerBase(UCompositeValue* Pointer)
{
	if (PointerBase)
	{
		PointerBase->RemoveListener(this);
	}

	PointerBase = Pointer;

	if (PointerBase)
	{
		PointerBase->AddListener(this);
	}

	MarkDirty();
}

float UCompositeValue::GetBaseValue() const
{
	if (PointerBase)
	{
		return PointerBase->GetValue();
	}

	return ConstantBase;
}

void UCompositeValue::MakeBaseConstant(float Constant)
{
	SetConstantBase(Constant);
	SetPointerBase(nullptr);
}

void UCompositeValue::MakeBasePointer(UCompositeValue* Pointer)
{
	SetPointerBase(Pointer);
}

//
// TRANSFORM
//

const FCompositeTransform& UCompositeValue::GetTransform() const
{
	return TransformFunction;
}

void UCompositeValue::SetTransform(const FCompositeTransform& Transform)
{
	TransformFunction = Transform;
	MarkDirty();
}

//
// MODIFIERS
//

bool UCompositeValue::RemoverPredicate(const FCompositeModifier& Modifier, bool PredicateResult)
{
	if (PredicateResult && Modifier.PointerValue)
	{
		Modifier.PointerValue->RemoveListener(this);
	}

	return PredicateResult;
}

void UCompositeValue::DefineMask(int32 Mask)
{
	ModifierMask = (ECompositeModifier)Mask;

	int32 Count = Modifiers.RemoveAll([this](FCompositeModifier& Modifier)
	{
		return RemoverPredicate(Modifier, (Modifier.Type & ModifierMask) != ECompositeModifier::None);
	});

	if (Count > 0)
	{
		MarkDirty();
	}
}

bool UCompositeValue::AddModifier(const FCompositeModifier& Modifier)
{
	if ((Modifier.Type & ModifierMask) != ECompositeModifier::None)
	{
		return false;
	}

	if (Modifier.PointerValue)
	{
		Modifier.PointerValue->AddListener(this);
	}

	Modifiers.Add(Modifier);
	MarkDirty();
	return true;
}

bool UCompositeValue::RemoveModifiers(const FName& Source)
{
	int32 Count = Modifiers.RemoveAll([this, Source](FCompositeModifier& Modifier)
	{
		return RemoverPredicate(Modifier, Modifier.Source == Source);
	});

	if (Count > 0)
	{
		MarkDirty();
	}

	return Count > 0;
}

void UCompositeValue::ClearModifiers()
{
	for (FCompositeModifier& Modifier : Modifiers)
	{
		if (Modifier.PointerValue)
		{
			Modifier.PointerValue->RemoveListener(this);
		}
	}

	Modifiers.Empty();
	MarkDirty();
}

//
// PARAMETERS
//

void UCompositeValue::MarkDirty()
{
	if (IsDirty)
	{
		return;
	}

	IsDirty = true;

	for (TPair<UCompositeValue*, int32>& Listener : Listeners)
	{
		Listener.Key->MarkDirty();
	}
}

ECompositeBounds UCompositeValue::GetBoundsType() const
{
	return BoundsType;
}

float UCompositeValue::GetLowerBound() const
{
	return LowerBound;
}

float UCompositeValue::GetUpperBound() const
{
	return UpperBound;
}

void UCompositeValue::SetBoundsType(ECompositeBounds Bounds)
{
	BoundsType = Bounds;
	MarkDirty();
}

void UCompositeValue::SetLowerBound(float Lower)
{
	LowerBound = Lower;
	MarkDirty();
}

void UCompositeValue::SetUpperBound(float Upper)
{
	UpperBound = Upper;
	MarkDirty();
}

void UCompositeValue::DefineBounds(ECompositeBounds Bounds, float Upper, float Lower)
{
	SetBoundsType(Bounds);
	SetLowerBound(Lower);
	SetUpperBound(Upper);
}

//
// CALCULATION
//

void UCompositeValue::Calculate()
{
	Value = TransformFunction.Evaluate(GetBaseValue());

	float PercentSum = 1;
	float InverseSum = 1;

	Modifiers.Sort([](const FCompositeModifier& A, const FCompositeModifier& B)
	{
		return (A.Order == B.Order) ? (A.Type < B.Type) : (A.Order < B.Order);
	});

	auto UpdateSums = [this, &PercentSum, &InverseSum]()
	{
		Value *= PercentSum;
		Value /= InverseSum;
		PercentSum = 1;
		InverseSum = 1;
	};

	for (const FCompositeModifier& Modifier : Modifiers)
	{
		switch (Modifier.Type)
		{
			case ECompositeModifier::PercentAddition:
			{
				PercentSum += Modifier.GetValue();

				if (PercentSum < 0)
				{
					PercentSum = 0;
				}

				break;
			}
			case ECompositeModifier::InverseAddition:
			{
				InverseSum += Modifier.GetValue();

				if (InverseSum < 0)
				{
					InverseSum = 0;
				}

				break;
			}
			case ECompositeModifier::Multiplication:
			{
				UpdateSums();

				Value *= Modifier.GetValue();

				break;
			}
			case ECompositeModifier::Division:
			{
				UpdateSums();

				Value /= Modifier.GetValue();

				break;
			}
			case ECompositeModifier::DirectAddition:
			{
				UpdateSums();

				Value += Modifier.GetValue();

				break;
			}
			default:
			{
				break;
			}
		}
	}

	UpdateSums();

	switch (BoundsType)
	{
		case ECompositeBounds::Minimum:
		{
			if (Value < LowerBound)
			{
				Value = LowerBound;
			}

			break;
		}
		case ECompositeBounds::Maximum:
		{
			if (Value > UpperBound)
			{
				Value = UpperBound;
			}

			break;
		}
		case ECompositeBounds::Interval:
		{
			if (Value < LowerBound)
			{
				Value = LowerBound;
			}

			if (Value > UpperBound)
			{
				Value = UpperBound;
			}

			break;
		}
		case ECompositeBounds::Exclusion:
		{
			if (Value < LowerBound && Value > UpperBound)
			{
				Value = UpperBound;
			}

			break;
		}
		default:
		{
			break;
		}
	}
}

float UCompositeValue::GetValue()
{
	if (IsDirty)
	{
		Calculate();
	}

	return Value;
}
