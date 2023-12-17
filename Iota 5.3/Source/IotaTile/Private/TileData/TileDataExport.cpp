// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileData/TileDataExport.h"
#include "Components/SceneComponent.h"
#include "Components/BillboardComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/ObjectSaveContext.h"
#include "UObject/SavePackage.h"
#include "UObject/Package.h"
#include "Engine/Level.h"
#include "Engine/World.h"
#include "Engine/Texture2D.h"
#include "AssetRegistry/AssetRegistryModule.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileDataExport)

ATileDataExport::ATileDataExport()
{
	SetHidden(true);
	SetCanBeDamaged(false);

	// Export scene component is a standard static scene component.
	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneRoot");
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
			, ID_TileExport("TileExport")
			, NAME_TileExport(NSLOCTEXT("SpriteCategory", "TileExport", "Tile Export"))
		{
			// Default constructor.
		}
	};

	static FConstructorStatics ConstructorStatics;

	// Define the billboard component subobject.
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>("HandleSprite");
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

#if WITH_EDITOR

void ATileDataExport::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);

	UWorld* World = GetWorld();
	ULevel* Level = GetLevel();

	if (GIsEditor && IsValid(World) && IsValid(Level))
	{
		if (Level->IsPersistentLevel() && !World->HasBegunPlay())
		{
			UTileDataAsset* AssetObject = nullptr;
			UPackage* AssetPackage = nullptr;
			bool bNewAsset = false;

			// If no data asset is linked but the export actor is authorized to create one, then
			// create a new data asset inside of the linked tilset's tile data content folder.
			if (DataAsset.IsNull() && Tileset.IsValid() && bAutoCreateAsset)
			{
				// Isolate the tileset name so that it can be used as a folder name.
				FString TilesetName = Tileset.ToString();
				FString ParentName = Tileset.RequestDirectParent().ToString();
				TilesetName.RemoveFromStart(ParentName + TEXT("."));

				// Create the new asset name and package path needed for the new asset.
				FString AssetName = World->GetMapName() + TEXT("_TileData");
				FString PackageName = TEXT("/Game/TileData/") + TilesetName + TEXT("/Tiles/") + AssetName;

				// Create a package for the new asset.
				AssetPackage = CreatePackage(*PackageName);
				AssetPackage->FullyLoad();

				// Create the new asset inside of the new package.
				EObjectFlags Flags = RF_Public | RF_Standalone | RF_Transactional;
				AssetObject = NewObject<UTileDataAsset>(AssetPackage, *AssetName, Flags);

				// Notify the asset registry of the new addition and mark it internally.
				FAssetRegistryModule::AssetCreated(AssetObject);
				bNewAsset = true;
			}

			// Attempt to extract the asset object and its package from the linked data asset.
			else if (UTileDataAsset* LinkAssetObject = DataAsset.LoadSynchronous())
			{
				AssetObject = LinkAssetObject;
				AssetPackage = AssetObject->GetPackage();
			}

			// Ensure that both the asset object and its package are valid before save.
			if (IsValid(AssetObject) && IsValid(AssetPackage))
			{
				AssetObject->Level = World;
				AssetObject->Schemes = Schemes;
				AssetObject->Tileset = Tileset;
				AssetObject->Objectives = Objectives;
				AssetObject->Portals.Empty(PortalActors.Num());
				AssetObject->Bounds.Empty(BoundActors.Num());
				// TODO - AreaGraph Export

				for (const ATilePortalActor* TilePortalActor : PortalActors)
				{
					// Export each tile portal actor as a tile portal structure.
					AssetObject->Portals.Emplace(TilePortalActor->GetTilePortal());
				}

				for (const ATileBoundActor* TileBoundActor : BoundActors)
				{
					// Export each tile bound actor as a tile bound structure.
					AssetObject->Bounds.Emplace(TileBoundActor->GetTileBound());
				}

				AssetPackage->MarkPackageDirty();

				// Get the package path relative to the local disk.
				FString FileName = FPackageName::LongPackageNameToFilename(
					AssetPackage->GetName(),
					FPackageName::GetAssetPackageExtension()
				);

				// Attempt to actually save the package and its contents. If the asset and package
				// were created as part of this function call, link them in the actor as well.
				if (UPackage::SavePackage(AssetPackage, AssetObject, *FileName, FSavePackageArgs()) && bNewAsset)
				{
					DataAsset = AssetObject;
				}
			}
		}
	}
}

#endif
