// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IotaCore/AssetActor.h"
#include "TileDoorBase.generated.h"

class USceneComponent;

/**
 * Base class for door actors spawned during tile map generation. Doors are selected for spawning
 * based on their tileset category and size. Doors without a tileset category will be ignored for
 * spawns. Child Blueprints should always fill out the Asset ID section under the Class Defaults,
 * unless the Blueprint is meant to be abstract.
 */
UCLASS(Abstract)
class IOTATILE_API ATileDoorBase : public AAssetActor
{
	GENERATED_BODY()

public:

	ATileDoorBase();

	/** Updates the general door sliding value. */
	virtual void Tick(float DeltaTime) override;

	/** Invoked during collision to begin tracking nearby actors. */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/** Invoked during collision to stop tracking nearby actors. */
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	/** Invoked whenever the door lock state updates. */
	UFUNCTION(BlueprintImplementableEvent, Category = "DoorState")
	void OnLockUpdate();

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
	 * Sets the door lock state and notifies Blueprints of the update.
	 *
	 * @param bLocked New door lock state.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true))
	void SetLocked(bool bLocked);

	/**
	 * Sets the door lock state and notifies Blueprints of the update.
	 *
	 * @param bSealed New door seal state.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true))
	void SetSealed(bool bSealed);

	/** Portal dimensions the door is meant to fit within. */
	UPROPERTY(BlueprintReadOnly, Category = "AssetID", EditDefaultsOnly)
	FIntPoint DoorSize = FIntPoint(4, 3);

	/** Length and maximum value of the door slide animation in seconds. */
	UPROPERTY(BlueprintSetter = SetSlideDuration, Category = "DoorSettings", EditAnywhere)
	float DoorSlideDuration = 1;

	/** Door lock state. While true, the door will close automatically. */
	UPROPERTY(BlueprintSetter = SetLocked, Category = "DoorSettings", EditAnywhere)
	bool bIsLocked = false;

	/** Door seal state. While true, the door becomes locked shut. */
	UPROPERTY(BlueprintSetter = SetSealed, Category = "DoorSettings", EditAnywhere)
	bool bIsSealed = false;

private:

	/** Door base component used as the door's pivot during placement. */
	UPROPERTY(BlueprintReadOnly, Category = "Components", VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> DoorBaseComponent;

	/** List of nearby actors used to open the door. */
	TArray<AActor*> TriggerActors;

	/** Current door slide animation state in seconds. */
	float DoorSlideState = 0;
};
