#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Composite/CompositeValue.h"
#include "ContextComponent.generated.h"


/// Base class for context components.
UCLASS(Abstract, Blueprintable, ClassGroup = (ActorContext), meta = (BlueprintSpawnableComponent))
class ACTORCONTEXT_API UContextComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	/// Represents the address of a Composite Value.
	typedef UCompositeValue** FCompositeVariable;

	/// Map of all Composite Value variables keyed to their own names.
	TMap<FName, FCompositeVariable> Variables;

public:

	// Default constructor.
	UContextComponent(const FObjectInitializer& Initializer);

protected:

	/// Generates the variable map using reflection.
	/// Passes to blueprints for further initialization.
	virtual void InitializeComponent() override;

	/// Invoked when the component needs to initialize via a level load or actor spawn. Initialize 
	/// is always called once, before Begin Play, and automatically instantiates Composite Value
	/// members so that they can be used in runtime.
	/// Implement this event to set up any relationships between composites.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Initialize"))
	void ReceiveInitialize();

public:

	/// Access a Composite Value variable by its property name.
	UFUNCTION(BlueprintPure)
	UCompositeValue* AccessMember(const FName& Name) const;
};
