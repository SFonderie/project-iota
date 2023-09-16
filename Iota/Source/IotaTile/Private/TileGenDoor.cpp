// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGenDoor.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileGenDoor)

ATileGenDoor::ATileGenDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set up the door base component as a standard static scene component.
	DoorBaseComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DoorRoot"));
	DoorBaseComponent->Mobility = EComponentMobility::Static;
	RootComponent = DoorBaseComponent;

	// Set up the door trigger to exceed the default Tile Portal Actor size.
	DoorTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	DoorTriggerBox->ShapeColor = FColor(100, 255, 100, 255);
	DoorTriggerBox->InitBoxExtent(FVector(300, 250, 150));

	DoorTriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	DoorTriggerBox->Mobility = EComponentMobility::Static;

	DoorTriggerBox->SetupAttachment(DoorBaseComponent);
	DoorTriggerBox->SetRelativeLocation(FVector(0, 0, 150));
}

void ATileGenDoor::Tick(float DeltaTime)
{
	// Increment or decrement and clamp the state.
	DoorState += DeltaTime * (IsDoorOpen() ? 1 : -1);
	DoorState = FMath::Clamp(DoorState, 0, DoorSlideDuration);

	// Continue with tick.
	Super::Tick(DeltaTime);
}

void ATileGenDoor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (CanActorTriggerDoor(OtherActor))
	{
		TriggerActors.Add(OtherActor);
	}

	Super::NotifyActorBeginOverlap(OtherActor);
}

void ATileGenDoor::NotifyActorEndOverlap(AActor* OtherActor)
{
	TriggerActors.Remove(OtherActor);
	Super::NotifyActorEndOverlap(OtherActor);
}

bool ATileGenDoor::CanActorTriggerDoor(AActor* Actor) const
{
	return Actor->IsA<APawn>();
}

bool ATileGenDoor::IsDoorOpen() const
{
	return !TriggerActors.IsEmpty() && !bIsLocked;
}

float ATileGenDoor::GetSlideRaw() const
{
	return DoorState;
}

float ATileGenDoor::GetSlideSmooth() const
{
	return FMath::SmoothStep(0.f, DoorSlideDuration, DoorState);
}
