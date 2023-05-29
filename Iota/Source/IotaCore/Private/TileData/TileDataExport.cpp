// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileData/TileDataExport.h"
#include "TileData/TilePortalActor.h"
#include "TileData/TilePortal.h"
#include "TileData/TileBoundActor.h"
#include "TileData/TileBound.h"
#include "Components/SceneComponent.h"
#include "Components/BillboardComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/ObjectSaveContext.h"
#include "UObject/SavePackage.h"
#include "UObject/Package.h"
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

void ATileDataExport::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);

	if (GIsEditor && DataAsset)
	{
		DataAsset->Level = GetWorld();
		DataAsset->Scheme = PreferredScheme;
		DataAsset->Portals.Empty(PortalActors.Num());
		DataAsset->Bounds.Empty(BoundActors.Num());
		DataAsset->Tileset = Tileset;

		for (const ATilePortalActor* TilePortalActor : PortalActors)
		{
			// Export each tile portal actor as a tile portal structure.
			DataAsset->Portals.Emplace(TilePortalActor->GetTilePortal());
		}

		for (const ATileBoundActor* TileBoundActor : BoundActors)
		{
			// Export each tile bound actor as a tile bound structure.
			DataAsset->Bounds.Emplace(TileBoundActor->GetTileBound());
		}

		// Access the data asset's package so that it can be saved.
		UPackage* AssetPackage = DataAsset->GetPackage();

		// Extract the data asset package location on the local disk.
		FString FileName = FPackageName::LongPackageNameToFilename(AssetPackage->GetName(), FPackageName::GetAssetPackageExtension());

		// Save the package and asset at the file location using default arguments.
		UPackage::SavePackage(AssetPackage, DataAsset, *FileName, FSavePackageArgs());
	}
}
