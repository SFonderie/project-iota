#include "Composite/CompositeValue.h"

UCompositeValue::UCompositeValue()
{
	// Default constructor.
}


//
// LISTENERS
//

void UCompositeValue::AddListener(UCompositeValue* Listener)
{
	if (!Listener)
	{
		return;
	}

	uint8* Count = Listeners.Find(Listener);

	if (Count)
	{
		(*Count)++;
		return;
	}

	Listeners.Emplace(Listener, 1);
}

void UCompositeValue::RemoveListener(UCompositeValue* Listener)
{
	if (!Listener)
	{
		return;
	}

	uint8* Count = Listeners.Find(Listener);

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

const TMap<UCompositeValue*, uint8>& UCompositeValue::GetListeners() const
{
	return Listeners;
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

void UCompositeValue::DefineTransform(const FCompositeTransform& Transform)
{
	SetTransform(Transform);
}

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

bool UCompositeValue::DelinkerPredicate(const FCompositeModifier& Modifier, bool bPredicate)
{
	if (bPredicate && Modifier.PointerValue)
	{
		Modifier.PointerValue->RemoveListener(this);
	}

	return bPredicate;
}

const TArray<FCompositeModifier>& UCompositeValue::GetModifiers() const
{
	return Modifiers;
}

void UCompositeValue::DefineMask(int32 Mask)
{
	ModifierMask = (ECompositeModifier) Mask;

	int32 Count = Modifiers.RemoveAll([&](const FCompositeModifier& Modifier)
	{
		return DelinkerPredicate(Modifier, (Modifier.Type & ModifierMask) != ECompositeModifier::None);
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

bool UCompositeValue::AddIntrinsic(const FCompositeModifier& Modifier, UCompositeValue* Pointer)
{
	FCompositeModifier Intrinsic = Modifier;
	Intrinsic.PointerValue = Pointer;
	Intrinsic.Source = FCompositeModifier::IntrinsicSource;

	return AddModifier(Intrinsic);
}

bool UCompositeValue::RemoveModifiers(const FName& Source)
{
	int32 Count = Modifiers.RemoveAll([&](const FCompositeModifier& Modifier)
	{
		return DelinkerPredicate(Modifier, Modifier.Source == Source);
	});

	if (Count > 0)
	{
		MarkDirty();
	}

	return Count > 0;
}

void UCompositeValue::ClearModifiers()
{
	int32 Count = Modifiers.RemoveAll([&](const FCompositeModifier& Modifier)
	{
		return DelinkerPredicate(Modifier, Modifier.Source != FCompositeModifier::IntrinsicSource);
	});

	if (Count > 0)
	{
		MarkDirty();
	}
}


//
// BOUNDS
//

void UCompositeValue::DefineBounds(const FCompositeBounds& Bounds)
{
	SetBounds(Bounds);
}

const FCompositeBounds& UCompositeValue::GetBounds() const
{
	return ValueBounds;
}

void UCompositeValue::SetBounds(const FCompositeBounds& Bounds)
{
	ValueBounds = Bounds;
	MarkDirty();
}


//
// CALCULATION
//

void UCompositeValue::MarkDirty()
{
	if (bDirty)
	{
		return;
	}

	bDirty = true;

	for (const TPair<UCompositeValue*, uint8>& Listener : Listeners)
	{
		Listener.Key->MarkDirty();
	}
}

void UCompositeValue::Calculate()
{
	Value = TransformFunction.Evaluate(GetBaseValue());

	float PercentSum = 1;
	float InverseSum = 1;

	Modifiers.Sort([](const FCompositeModifier& A, const FCompositeModifier& B)
	{
		return (A.Order == B.Order) ? (A.Type < B.Type) : (A.Order < B.Order);
	});

	auto UpdateSums = [&]()
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

	Value = ValueBounds.Clamp(Value);
}

float UCompositeValue::GetValue()
{
	if (bDirty)
	{
		Calculate();
	}

	return Value;
}
