// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TileGen/TileAssetActor.h"
#include "TileDoorActor.generated.h"

class USceneComponent;

/** Base class for door actors spawned during level generation. */
UCLASS(Abstract)
class IOTATILE_API ATileDoorActor : public ATileAssetActor
{
	GENERATED_BODY()

public:

	ATileDoorActor();

	/** Updates the general door sliding value. */
	virtual void Tick(float DeltaTime) override;

	/** Invoked during collision to begin tracking nearby actors. */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/** Invoked during collision to stop tracking nearby actors. */
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	/**
	 * Determines if the given actor is allowed to interact with the door. The actor must be a
	 * Pawn by default, but child classes can choose override or extend this rule as needed.
	 *
	 * @param Actor Actor to query for access.
	 * @return True if the actor can open the door.
	 */
	UFUNCTION(BlueprintPure, Category = "DoorState")
	virtual bool CanActorTriggerDoor(AActor* Actor) const;

	/**
	 * Determines if the door is open or in the process of opening. If the door is locked, this
	 * method will always return false.
	 *
	 * @return True if the door is unlocked and open.
	 */
	UFUNCTION(BlueprintPure, Category = "DoorState")
	bool IsDoorOpen() const;

	/**
	 * Returns the raw door slide animation state as a time value in seconds. Animators can use
	 * this method as a basis for custom animation curves.
	 *
	 * The returned value is limited to the interval [0, DoorSlideDuration] seconds.
	 *
	 * @return Raw door slide animation state in seconds.
	 */
	UFUNCTION(BlueprintPure, Category = "DoorState")
	float GetSlideRaw() const;

	/**
	 * Returns the door slide animation state as a smooth step value. For most simple doors, this
	 * method can be used directly to create a door animation without the use of a curve.
	 *
	 * The returned value is limited to the interval [0, 1] with no units.
	 *
	 * @return Smoothed door slide animation state.
	 */
	UFUNCTION(BlueprintPure, Category = "DoorState")
	float GetSlideSmooth() const;

	/**
	 * Sets the length and maximum value of the door slide animation in seconds. If the provided
	 * value is less than or equal to 0 seconds, the duration will not be updated.
	 *
	 * @param NewDuration New duration for the door slide animation in seconds.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true))
	void SetSlideDuration(float NewDuration);

	/**
	 * Returns the length and maximum value of the door slide animation in seconds.
	 *
	 * @return Length of the door slide animation in seconds.
	 */
	float GetSlideDuration() const
	{
		return DoorSlideDuration;
	}

	/** Portal dimensions the door is meant to fit within. */
	UPROPERTY(Category = "AssetID", EditDefaultsOnly)
	FIntPoint DoorSize = FIntPoint(4, 3);

	/** Door lock state. While true, the door will close automatically. */
	UPROPERTY(BlueprintReadWrite, Category = "DoorSettings", EditAnywhere)
	bool bIsLocked = false;

private:

	/** Door base component used as the door's pivot during placement. */
	UPROPERTY(BlueprintReadOnly, Category = "Components", VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> DoorBaseComponent;

	/** Length and maximum value of the door slide animation in seconds. */
	UPROPERTY(BlueprintSetter = SetSlideDuration, Category = "DoorSettings", EditAnywhere)
	float DoorSlideDuration = 1;

	/** List of nearby actors used to open the door. */
	TArray<AActor*> TriggerActors;

	/** Current door slide animation state in seconds. */
	float DoorSlideState = 0;
};
