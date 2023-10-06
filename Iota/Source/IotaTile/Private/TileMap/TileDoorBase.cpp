// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileMap/TileDoorBase.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileDoorBase)

ATileDoorBase::ATileDoorBase()
{
	PrimaryActorTick.bCanEverTick = true;

	DoorBaseComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DoorPivot"));
	DoorBaseComponent->Mobility = EComponentMobility::Static;
	RootComponent = DoorBaseComponent;
}

void ATileDoorBase::Tick(float DeltaTime)
{
	DoorSlideState += DeltaTime * (IsDoorOpen() ? 1 : -1);
	DoorSlideState = FMath::Clamp(DoorSlideState, 0.f, DoorSlideDuration);

	Super::Tick(DeltaTime);
}

void ATileDoorBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (CanActorTriggerDoor(OtherActor))
	{
		TriggerActors.Emplace(OtherActor);
	}

	Super::NotifyActorBeginOverlap(OtherActor);
}

void ATileDoorBase::NotifyActorEndOverlap(AActor* OtherActor)
{
	TriggerActors.Remove(OtherActor);
	Super::NotifyActorEndOverlap(OtherActor);
}

bool ATileDoorBase::CanActorTriggerDoor(AActor* Actor) const
{
	return IsValid(Actor) && Actor->IsA<APawn>();
}

bool ATileDoorBase::IsDoorOpen() const
{
	return !TriggerActors.IsEmpty() && !bIsLocked && !bIsSealed;
}

float ATileDoorBase::GetSlideRaw() const
{
	return DoorSlideState;
}

float ATileDoorBase::GetSlideSmooth() const
{
	return FMath::SmoothStep(0.f, DoorSlideDuration, DoorSlideState);
}

void ATileDoorBase::SetSlideDuration(float NewDuration)
{
	DoorSlideDuration = NewDuration > 0 ? NewDuration : DoorSlideDuration;
}

float ATileDoorBase::GetSlideDuration() const
{
	return DoorSlideDuration;
}
