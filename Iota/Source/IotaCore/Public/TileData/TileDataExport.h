// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileData/TileDataAsset.h"
#include "TileDataExport.generated.h"

class ATilePortalActor;
class ATileBoundActor;
class USceneComponent;
class UBillboardComponent;

/** Exports tile level data to a linked asset. */
UCLASS()
class IOTACORE_API ATileDataExport : public AActor
{
	GENERATED_BODY()

public:

	/** Export to this data asset on save. */
	UPROPERTY(Category = "ExportSettings", EditAnywhere)
	TObjectPtr<UTileDataAsset> DataAsset;

	/** Export the tile with this preferred scheme. */
	UPROPERTY(Category = "ExportSettings", EditAnywhere)
	ETileScheme PreferredScheme = ETileScheme::Stopper;

	/** Tile portal actors that should be exported. */
	UPROPERTY(Category = "ExportSettings", EditAnywhere)
	TArray<TObjectPtr<ATilePortalActor>> PortalActors;

	/** Tile bound actors that should be exported. */
	UPROPERTY(Category = "ExportSettings", EditAnywhere)
	TArray<TObjectPtr<ATileBoundActor>> BoundActors;

	ATileDataExport();

private:

	/** Scene component used as the actor root for all builds. */
	UPROPERTY(BlueprintReadOnly, Category = "ExportActor", VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> SceneComponent;

#if WITH_EDITORONLY_DATA

	/** Billboard component used to access the export actor in-editor. */
	UPROPERTY()
	TObjectPtr<UBillboardComponent> SpriteComponent;

#endif

public:

	/** @return Scene component used as the actor root for all builds. */
	USceneComponent* GetSceneComponent() const
	{
		return SceneComponent;
	}

#if WITH_EDITORONLY_DATA

	/** @return Billboard component used to access the export actor in-editor. */
	UBillboardComponent* GetSpriteComponent() const
	{
		return SpriteComponent;
	}

#endif

	/** Actually executes the tile level export to the linked data asset. */
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;
};
