// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileData/TileDataExport.h"
#include "Components/SceneComponent.h"
#include "Components/BillboardComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/ObjectSaveContext.h"
#include "UObject/SavePackage.h"
#include "UObject/Package.h"
#include "Engine/Texture2D.h"
#include "AssetRegistry/AssetRegistryModule.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileDataExport)

ATileDataExport::ATileDataExport()
{
	SetHidden(true);
	SetCanBeDamaged(false);

	// Export scene component is a standard static scene component.
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SceneComponent->Mobility = EComponentMobility::Static;
	RootComponent = SceneComponent;

#if WITH_EDITORONLY_DATA

	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UTexture2D> SpriteObject;
		FName ID_TileExport;
		FText NAME_TileExport;

		FConstructorStatics()
			: SpriteObject(TEXT("/Engine/EditorResources/S_TriggerSphere"))
			, ID_TileExport(TEXT("TileExport"))
			, NAME_TileExport(NSLOCTEXT("SpriteCategory", "TileExport", "Tile Export"))
		{
			// Default constructor.
		}
	};

	static FConstructorStatics ConstructorStatics;

	// Define the billboard component subobject.
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("HandleSprite"));
	SpriteComponent->Sprite = ConstructorStatics.SpriteObject.Get();
	SpriteComponent->bIsScreenSizeScaled = true;

	// Set up some other properties for the sprite.
	SpriteComponent->SpriteInfo.Category = ConstructorStatics.ID_TileExport;
	SpriteComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_TileExport;
	SpriteComponent->bHiddenInGame = false;

	// Connect the billboard to the plane center.
	SpriteComponent->SetupAttachment(SceneComponent);

#endif
}

void ATileDataExport::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);

	if (GIsEditor)
	{
		if (DataAsset.IsNull() && bAutoCreateAsset)
		{
			// Isolate the tileset tag name.
			FString TilesetName = Tileset.ToString();
			FString ParentName = Tileset.RequestDirectParent().ToString();
			TilesetName.RemoveFromStart(ParentName + TEXT("."));

			// Create the package name from the level and a default folder.
			FString AssetName = GetWorld()->GetMapName() + TEXT("_TileData");
			FString PackageName = TEXT("/Game/TileData/") + TilesetName + TEXT("/Tiles/") + AssetName;

			// Create a new package for the tile data asset.
			UPackage* NewPackage = CreatePackage(*PackageName);
			NewPackage->FullyLoad();

			// Create the actual tile data asset, register it, and then save the reference.
			UTileDataAsset* NewDataAsset = NewObject<UTileDataAsset>(NewPackage, *AssetName, RF_Public | RF_Standalone);
			FAssetRegistryModule::AssetCreated(NewDataAsset);
			DataAsset = NewDataAsset;
		}

		if (UTileDataAsset* DataAssetObject = DataAsset.LoadSynchronous())
		{
			DataAssetObject->Level = GetWorld();
			DataAssetObject->Schemes = PreferredSchemes;
			DataAssetObject->Tileset = Tileset;
			DataAssetObject->Objectives = Objectives;
			DataAssetObject->Portals.Empty(PortalActors.Num());
			DataAssetObject->Bounds.Empty(BoundActors.Num());

			for (const ATilePortalActor* TilePortalActor : PortalActors)
			{
				// Export each tile portal actor as a tile portal structure.
				DataAssetObject->Portals.Emplace(TilePortalActor->GetTilePortal());
			}

			for (const ATileBoundActor* TileBoundActor : BoundActors)
			{
				// Export each tile bound actor as a tile bound structure.
				DataAssetObject->Bounds.Emplace(TileBoundActor->GetTileBound());
			}

			if (UPackage* AssetPackage = DataAssetObject->GetPackage())
			{
				// Mark the package for saving.
				AssetPackage->MarkPackageDirty();

				// Get the package path relative to the local disk.
				FString FileName = FPackageName::LongPackageNameToFilename(
					AssetPackage->GetName(), 
					FPackageName::GetAssetPackageExtension()
				);

				// Attempt to actually save the package and its contents.
				UPackage::SavePackage(AssetPackage, DataAssetObject, *FileName, FSavePackageArgs());
			}
		}
	}
}
