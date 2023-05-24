// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "Tileset/TileDataExport.h"
#include "Tileset/TilePortalActor.h"
#include "Tileset/TileBoundActor.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/ObjectSaveContext.h"
#include "UObject/Package.h"
#include "Components/SceneComponent.h"
#include "Components/BillboardComponent.h"
#include "Engine/Texture2D.h"

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

#if WITH_EDITOR

void ATileDataExport::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);

	if (DataAsset)
	{
		DataAsset->Level = GetWorld();
		DataAsset->Scheme = PreferredScheme;
		DataAsset->Portals.Empty(PortalActors.Num());
		DataAsset->Bounds.Empty(BoundActors.Num());

		for (const ATilePortalActor* TilePortalActor : PortalActors)
		{
			DataAsset->Portals.Emplace(TilePortalActor->GetTilePortal());
		}

		for (const ATileBoundActor* TileBoundActor : BoundActors)
		{
			DataAsset->Bounds.Emplace(TileBoundActor->GetTileBound());
		}

		DataAsset->ValidateForSave();

		// Access the data asset's package so we can save it.
		UPackage* AssetPackage = DataAsset->GetPackage();

		// Extract the data asset's file name on the local disk.
		FString FileName = FPackageName::LongPackageNameToFilename(
			AssetPackage->GetName(), 
			FPackageName::GetAssetPackageExtension()
		);

		// Attempt to save the package using a standard call.
		UPackage::SavePackage(
			AssetPackage, DataAsset, RF_Public | RF_Standalone, *FileName, 
			GError, nullptr, true, true, SAVE_NoError
		);
	}
}

#endif
