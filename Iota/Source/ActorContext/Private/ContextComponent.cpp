#include "ContextComponent.h"

UContextComponent::UContextComponent(const FObjectInitializer& Initializer) : Super(Initializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UContextComponent::InitializeComponent()
{
	Super::InitializeComponent();

	const FName& CompositeType = UCompositeValue::StaticClass()->GetFName();

	for (TFieldIterator<FObjectProperty> Iterator(GetClass()); Iterator; ++Iterator)
	{
		const FName& PropertyType = (*Iterator)->PropertyClass->GetFName();
		const FName& PropertyName = (*Iterator)->GetFName();

		if (PropertyType == CompositeType)
		{
			FCompositeVariable Variable = (*Iterator)->ContainerPtrToValuePtr<UCompositeValue*>(this);

			*Variable = NewObject<UCompositeValue>(this);
			Variables.Emplace(PropertyName, Variable);
		}
	}

	ReceiveInitialize();
}

UCompositeValue* UContextComponent::AccessMember(const FName& Name) const
{
	const FCompositeVariable* Variable = Variables.Find(Name);

	if (Variable)
	{
		return **Variable;
	}

	return nullptr;
}
