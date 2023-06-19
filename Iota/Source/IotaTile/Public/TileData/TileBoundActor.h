// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileBoundActor.generated.h"

struct FTileBound;
class UBoxComponent;

/** Represents an oriented tile bounding box in the world. */
UCLASS()
class IOTATILE_API ATileBoundActor : public AActor
{
	GENERATED_BODY()

public:

	ATileBoundActor();

	/**
	 * Converts the actor box component into a tile bound data structure.
	 *
	 * @return The actor bounding box converted into a data-only form.
	 */
	UFUNCTION(BlueprintPure, Category = "TileBound")
	FTileBound GetTileBound() const;

private:

	/** Box component used to show the tile bound in-editor. */
	UPROPERTY(BlueprintReadOnly, Category = "TileBound", VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> BoxComponent;

public:

	/** @return Box component used to show the tile bound in-editor. */
	UBoxComponent* GetBoxComponent() const
	{
		return BoxComponent;
	}

#if WITH_EDITOR

private:

	/**
	 * Invoked whenever the user changes the actor scale using the editor viewport widget. Tile
	 * bound actor uses this method to scale the bounding box extent instead of its transform.
	 */
	virtual void EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown) override;

#endif
};
