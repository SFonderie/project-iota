// Copyright Sydney Fonderie. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileBoundActor.generated.h"

struct FTileBound;
class UBoxComponent;

/** Represents a tile collision bound in the world. */
UCLASS()
class ATileBoundActor : public AActor
{
	GENERATED_BODY()

public:

	ATileBoundActor();

	/**
	 * Converts the actor box component into a tile bound structure.
	 *
	 * @return The actor bounding box converted into tile data.
	 */
	UFUNCTION(BlueprintPure, Category = "Tile|TileBound")
	FTileBound GetTileBound() const;

private:

	/** Box component used to show the tile bound in-editor. */
	UPROPERTY(BlueprintReadOnly, Category = "TileBound", VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> BoxComponent;

#if WITH_EDITOR

	/**
	 * Invoked whenever the user changes the actor scale using the editor viewport widget. Tile
	 * bound actor uses this method to scale the bounding box extent instead of its transform.
	 */
	virtual void EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown) override;

#endif
};
