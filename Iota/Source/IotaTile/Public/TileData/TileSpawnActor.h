// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileSpawnActor.generated.h"

class UBoxComponent;
class UNavigationQueryFilter;
class ANavigationData;

/** Represents a spawn volume within a tile. */
UCLASS()
class IOTATILE_API ATileSpawnActor : public AActor
{
	GENERATED_BODY()

public:

	ATileSpawnActor();

private:

	/** Box component used to show the tile spawn volume in-editor. */
	UPROPERTY(BlueprintReadOnly, Category = "TileSpawn", VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> BoxComponent;

	/** Filter class used when making navigation queries. */
	UPROPERTY(BlueprintReadOnly, Category = "TileSpawn", EditAnywhere, meta = (AllowPrivateAccess = true))
	TSubclassOf<UNavigationQueryFilter> FilterClass;

public:

	/**
	 * Selects a random location within the spawn volume and projects it down to the navigation
	 * data parameter, or to the world default if none is provided.
	 *
	 * @param NavData Navigation data to use in place of the world default value.
	 * @param OutLocation Random spawn location projected to the world navigation data.
	 * @return True if the projection was successful.
	 */
	UFUNCTION(BlueprintPure = false)
	bool GenerateSpawnLocation(ANavigationData* NavData, FVector& OutLocation) const;

	/** @return Box component used to show the tile spawn volume in-editor. */
	UBoxComponent* GetBoxComponent() const
	{
		return BoxComponent;
	}

#if WITH_EDITOR

private:

	/**
	 * Invoked whenever the user changes the actor scale using the editor viewport widget. Tile
	 * spawn volume uses this method to scale the bounding box extent instead of its transform.
	 */
	virtual void EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown) override;

#endif
};
