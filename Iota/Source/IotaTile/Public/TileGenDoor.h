// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TileGenActor.h"
#include "TileGenDoor.generated.h"

class USceneComponent;
class UBoxComponent;

/** Base class for door actors spawned during level generation. */
UCLASS(Abstract)
class IOTATILE_API ATileGenDoor : public ATileGenActor
{
	GENERATED_BODY()

public:

	ATileGenDoor();

	/** Updates the door state value. */
	virtual void Tick(float DeltaTime) override;

	/** Invoked during collision to begin tracking nearby actors. */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/** Invoked during collision to stop tracking nearby actors. */
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	/** Portal dimensions this door type is meant to fit into. */
	UPROPERTY(Category = "AssetID", EditDefaultsOnly)
	FIntPoint DoorSize = FIntPoint(4, 3);

	/** Length and maximum value of the door slide animation measured in seconds. */
	UPROPERTY(BlueprintReadWrite, Category = "TileDoor", EditAnywhere)
	float DoorSlideDuration = 0.2f;

	/** Door lock state. While true, the door will close and remain closed. */
	UPROPERTY(BlueprintReadWrite, Category = "TileDoor", EditAnywhere)
	bool bIsLocked = false;

protected:

	/**
	 * Determines if the given actor is allowed to interact with the door trigger.
	 *
	 * @return True if the actor can open the door.
	 */
	UFUNCTION(BlueprintPure, Category = "TileDoor")
	bool CanActorTriggerDoor(AActor* Actor) const;

	/**
	 * Determines if the door is open or in the process of opening. If the door has been locked,
	 * this value will always return false.
	 *
	 * @return True if the door is currently open or opening.
	 */
	UFUNCTION(BlueprintPure, Category = "TileDoor|State")
	bool IsDoorOpen() const;

	/**
	 * Returns the door slide state as a time value in seconds. Animators can use this function to
	 * get an input for animation curves.
	 *
	 * @return Raw door slide state value.
	 */
	UFUNCTION(BlueprintPure, Category = "TileDoor|State")
	float GetSlideRaw() const;

	/**
	 * Returns the door slide state as a smooth step value. For most simple doors, the output from
	 * this function can be used to create a complete animation without the use of a curve.
	 *
	 * @return Smooth-stepped door slide state value.
	 */
	UFUNCTION(BlueprintPure, Category = "TileDoor|State")
	float GetSlideSmooth() const;

private:

	/** Door base component positioned at the door's center on the level floor. */
	UPROPERTY(BlueprintReadOnly, Category = "Components", VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> DoorBaseComponent;

	/** Box component used to open and close the door when other actors move nearby. */
	UPROPERTY(BlueprintReadOnly, Category = "Components", VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> DoorTriggerBox;

public:

	/** @return Door base component positioned at the door's center on the level floor. */
	USceneComponent* GetDoorBase() const
	{
		return DoorBaseComponent;
	}

	/** @return Box component used to open and close the door when other actors move nearby. */
	UBoxComponent* GetDoorTrigger() const
	{
		return DoorTriggerBox;
	}

private:

	/** Validated list of actors currently inside the door trigger. */
	TArray<AActor*> TriggerActors;

	/** Current door slide progress. */
	float DoorState = 0;
};
