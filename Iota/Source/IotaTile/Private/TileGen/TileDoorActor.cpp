// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGen/TileDoorActor.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileDoorActor)

ATileDoorActor::ATileDoorActor()
{
	PrimaryActorTick.bCanEverTick = true;

	DoorBaseComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DoorPivot"));
	DoorBaseComponent->Mobility = EComponentMobility::Static;
	RootComponent = DoorBaseComponent;
}

void ATileDoorActor::Tick(float DeltaTime)
{
	DoorSlideState += DeltaTime * (IsDoorOpen() ? 1 : -1);
	DoorSlideState = FMath::Clamp(DoorSlideState, 0.f, DoorSlideDuration);

	Super::Tick(DeltaTime);
}

void ATileDoorActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (CanActorTriggerDoor(OtherActor))
	{
		TriggerActors.Emplace(OtherActor);
	}

	Super::NotifyActorBeginOverlap(OtherActor);
}

void ATileDoorActor::NotifyActorEndOverlap(AActor* OtherActor)
{
	TriggerActors.Remove(OtherActor);
	Super::NotifyActorEndOverlap(OtherActor);
}

bool ATileDoorActor::CanActorTriggerDoor(AActor* Actor) const
{
	return IsValid(Actor) && Actor->IsA<APawn>();
}

bool ATileDoorActor::IsDoorOpen() const
{
	return !TriggerActors.IsEmpty() && !bIsLocked;
}

float ATileDoorActor::GetSlideRaw() const
{
	return DoorSlideState;
}

float ATileDoorActor::GetSlideSmooth() const
{
	return FMath::SmoothStep(0.f, DoorSlideDuration, DoorSlideState);
}

void ATileDoorActor::SetSlideDuration(float NewDuration)
{
	DoorSlideDuration = NewDuration > 0 ? NewDuration : DoorSlideDuration;
}
