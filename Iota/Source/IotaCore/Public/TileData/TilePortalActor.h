// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileData/TilePortal.h"
#include "TilePortalActor.generated.h"

class UBoxComponent;
class UBillboardComponent;
class UArrowComponent;

/** Represents a tile connection portal in the world. */
UCLASS()
class IOTACORE_API ATilePortalActor : public AActor
{
	GENERATED_BODY()

public:

	ATilePortalActor();

	/**
	 * Converts the actor into a tile portal data structure.
	 *
	 * @return The actor transform converted into a data-only form.
	 */
	UFUNCTION(BlueprintPure, Category = "TilePortal")
	FTilePortal GetTilePortal() const;

private:

	/** Box component used to show the portal plane in-editor. */
	UPROPERTY(BlueprintReadOnly, Category = "TilePortal", VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> PlaneComponent;

#if WITH_EDITORONLY_DATA

	/** Billboard component used to show the portal location in-editor. */
	UPROPERTY()
	TObjectPtr<UBillboardComponent> SpriteComponent;

	/** Arrow component used to show the portal direction in-editor. */
	UPROPERTY()
	TObjectPtr<UArrowComponent> ArrowComponent;

#endif

#if WITH_EDITOR

	/**
	 * Invoked whenever the user changes the actor scale using the editor viewport widget. Tile
	 * portal actor uses this method to scale the bounding box extent instead of its transform.
	 */
	virtual void EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown) override;

#endif
};
