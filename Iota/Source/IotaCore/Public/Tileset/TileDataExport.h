// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tileset/TileDataAsset.h"
#include "TileDataExport.generated.h"

/** Exports tile level data to a linked asset. */
UCLASS()
class IOTACORE_API ATileDataExport : public AActor
{
	GENERATED_BODY()

public:

	/** Export to this data asset on save. */
	UPROPERTY(Category = "ExportSettings", EditAnywhere)
	TObjectPtr<class UTileDataAsset> DataAsset;

	/** Export the tile with this preferred scheme. */
	UPROPERTY(Category = "ExportSettings", EditAnywhere)
	ETileScheme PreferredScheme = ETileScheme::Stopper;

	/** Tile portal actors that should be exported. */
	UPROPERTY(Category = "ExportSettings", EditAnywhere)
	TArray<TObjectPtr<class ATilePortalActor>> PortalActors;

	/** Tile bound actors that should be exported. */
	UPROPERTY(Category = "ExportSettings", EditAnywhere)
	TArray<TObjectPtr<class ATileBoundActor>> BoundActors;

	ATileDataExport();

private:

	/** Scene component used as the actor root for all builds. */
	UPROPERTY(BlueprintReadOnly, Category = "ExportActor", VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<class USceneComponent> SceneComponent;

#if WITH_EDITORONLY_DATA

	/** Billboard component used to access the export actor in-editor. */
	UPROPERTY()
	TObjectPtr<class UBillboardComponent> SpriteComponent;

#endif

public:

	/** @return Scene component used as the actor root for all builds. */
	class USceneComponent* GetSceneComponent() const
	{
		return SceneComponent;
	}

#if WITH_EDITORONLY_DATA

	/** @return Billboard component used to access the export actor in-editor. */
	class UBillboardComponent* GetSpriteComponent() const
	{
		return SpriteComponent;
	}

#endif

#if WITH_EDITOR

	/** Actually executes the tile level export to the linked data asset. */
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;

#endif
};
