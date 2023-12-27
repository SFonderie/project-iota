// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileData/TileScheme.h"
#include "GameplayTagContainer.h"
#include "TileDataExport.generated.h"

class UTileDataAsset;
class ATileBoundActor;
class ATilePortalActor;
class ATileNodeActor;
class USceneComponent;
class UBillboardComponent;

/** Exports tile level data to a linked asset. */
UCLASS()
class ATileDataExport : public AActor
{
	GENERATED_BODY()

public:

	/** Export to this data asset on save. */
	UPROPERTY(Category = "ExportLevel", EditAnywhere, meta = (NoResetToDefault))
	TSoftObjectPtr<UTileDataAsset> DataAsset;

	/** Export the tile with these preferred schemes. */
	UPROPERTY(Category = "ExportLevel", EditAnywhere, meta = (Bitmask, BitmaskEnum = ETileScheme))
	int32 Schemes = 1 << ETileScheme::Connector;

	/** Tileset to which the tile belongs. */
	UPROPERTY(Category = "ExportData", EditAnywhere, meta = (Categories = "Tileset"))
	FGameplayTag Tileset = FGameplayTag::RequestGameplayTag("Tileset.Whitebox");

	/** Objectives to which the tile belongs. If empty, the tile will appear in all objectives. */
	UPROPERTY(Category = "ExportData", EditAnywhere, meta = (Categories = "Objective"))
	FGameplayTagContainer Objectives;

	/** Tile portal actors that should be exported. */
	UPROPERTY(Category = "ExportActors", EditAnywhere, meta = (DisplayAfter = bAutoLocateActors))
	TArray<TObjectPtr<ATilePortalActor>> PortalActors;

	/** Tile bound actors that should be exported. */
	UPROPERTY(Category = "ExportActors", EditAnywhere, meta = (DisplayAfter = bAutoLocateActors))
	TArray<TObjectPtr<ATileBoundActor>> BoundActors;

	/** Area node actors from which to build a local area graph. */
	UPROPERTY(Category = "ExportActors", EditAnywhere, meta = (DisplayAfter = bAutoLocateActors))
	TArray<TObjectPtr<ATileNodeActor>> NodeActors;

	/** True if the export should automatically create a data asset when none is linked. */
	UPROPERTY(Category = "ExportLevel", EditAnywhere)
	bool bAutoCreateAsset = true;

	/** True if the export should automatically fill the actor arrays when saved. */
	UPROPERTY(Category = "ExportActors", EditAnywhere)
	bool bAutoLocateActors = true;

	ATileDataExport();

private:

	/** Scene component used as the actor root for all builds. */
	UPROPERTY(BlueprintReadOnly, Category = "TileExport", VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> SceneComponent;

#if WITH_EDITORONLY_DATA

	/** Billboard component used to access the export actor in-editor. */
	UPROPERTY()
	TObjectPtr<UBillboardComponent> SpriteComponent;

#endif

#if WITH_EDITOR

protected:

	/** Actually exports the actor into the linked data asset. */
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;

#endif
};