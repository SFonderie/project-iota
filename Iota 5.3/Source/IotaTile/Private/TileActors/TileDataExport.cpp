// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileActors/TileDataExport.h"
#include "TileActors/TileBoundActor.h"
#include "TileActors/TileNodeActor.h"
#include "TileActors/TilePortalActor.h"
#include "TileData/TileDataAsset.h"
#include "Components/SceneComponent.h"
#include "Components/BillboardComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/ObjectSaveContext.h"
#include "UObject/SavePackage.h"
#include "UObject/Package.h"
#include "Engine/CollisionProfile.h"
#include "Engine/Level.h"
#include "Engine/World.h"
#include "Engine/Texture2D.h"
#include "Misc/PackageName.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "EngineUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileDataExport)

ATileDataExport::ATileDataExport()
{
	bIsEditorOnlyActor = true;
	SetActorHiddenInGame(true);
	SetCanBeDamaged(false);

	// Export scene component is a standard static scene component.
	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SceneComponent->Mobility = EComponentMobility::Static;
	RootComponent = SceneComponent;

#if WITH_EDITORONLY_DATA

	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UTexture2D> SpriteObject;

		FConstructorStatics() : SpriteObject(TEXT("/Engine/EditorResources/S_TriggerSphere"))
		{
			// Default constructor.
		}
	};

	static FConstructorStatics ConstructorStatics;

	// Define the billboard component subobject.
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>("HandleSprite");

	if (IsValid(SpriteComponent))
	{
		SpriteComponent->Sprite = ConstructorStatics.SpriteObject.Get();
		SpriteComponent->bIsScreenSizeScaled = true;
		SpriteComponent->bHiddenInGame = false;

		// Connect the billboard to the root component.
		SpriteComponent->SetupAttachment(SceneComponent);
	}

#endif
}

#if WITH_EDITOR

void ATileDataExport::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);

	UWorld* World = GetWorld();
	ULevel* Level = GetLevel();

	// Data export should only ever occur on saves made in the editor. The export actor must also
	// exist on the current persistent level, and the editor cannot be running a PIE instance.
	if (GIsEditor && IsValid(World) && IsValid(Level) && !World->HasBegunPlay() && Level->IsPersistentLevel())
	{
		UTileDataAsset* AssetObject = nullptr;
		UPackage* AssetPackage = nullptr;
		bool bNewAsset = false;

		// If no data asset is linked but the export actor is authorized to create one, then create
		// a new data asset inside of the linked tilset's tile data content folder.
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
			if (bAutoLocateActors)
			{
				PortalActors.Empty();
				BoundActors.Empty();
				NodeActors.Empty();

				// Locate and store references to all tile portal actors in the world.
				for (TActorIterator<ATilePortalActor> Iterator(World); Iterator; ++Iterator)
				{
					PortalActors.Emplace(*Iterator);
				}

				// Locate and store references to all tile bound actors in the world.
				for (TActorIterator<ATileBoundActor> Iterator(World); Iterator; ++Iterator)
				{
					BoundActors.Emplace(*Iterator);
				}

				// Locate and store references to all area node actors in the world.
				for (TActorIterator<ATileNodeActor> Iterator(World); Iterator; ++Iterator)
				{
					NodeActors.Emplace(*Iterator);
				}
			}

			// Pass values to the loaded data asset.
			AssetObject->Level = World;
			AssetObject->Schemes = Schemes;
			AssetObject->Tileset = Tileset;
			AssetObject->Objectives = Objectives;
			AssetObject->Portals.Empty(PortalActors.Num());
			AssetObject->Bounds.Empty(BoundActors.Num());
			AssetObject->AreaNodes.Empty(NodeActors.Num());
			AssetObject->AreaExits.Empty(PortalActors.Num());

			// Export each tile portal actor as a tile portal structure.
			for (const ATilePortalActor* TilePortalActor : PortalActors)
			{
				if (IsValid(TilePortalActor))
				{
					AssetObject->Portals.Emplace(TilePortalActor->GetTilePortal());
				}
			}

			// Export each tile bound actor as a tile bound structure.
			for (const ATileBoundActor* TileBoundActor : BoundActors)
			{
				if (IsValid(TileBoundActor))
				{
					AssetObject->Bounds.Emplace(TileBoundActor->GetTileBound());
				}
			}

			// Export each area node actor as an area node data structure. For each node, also 
			// generate connections to form a local area graph for the target tile.
			for (const ATileNodeActor* AreaNodeActor : NodeActors)
			{
				if (IsValid(AreaNodeActor))
				{
					int32 NewIndex = AssetObject->AreaNodes.Emplace(AreaNodeActor->GetNodeData());

					// Iterate through each previous node and attempt to form a connection via a
					// line trace. If the trace returns no hits, then a connection is added.
					for (int32 Previous = 0; Previous < NewIndex; Previous++)
					{
						if (!World->LineTraceTestByProfile(
							AssetObject->AreaNodes[NewIndex].Location,
							AssetObject->AreaNodes[Previous].Location,
							UCollisionProfile::BlockAll_ProfileName))
						{
							FTileNode::MakeConnection(AssetObject->AreaNodes, NewIndex, Previous);
						}
					}
				}
			}

			// Find the area node closest to each tile portal and save its index as an exit value.
			for (const FTilePortal& Portal : AssetObject->Portals)
			{
				float MinDistance = UE_FLOAT_HUGE_DISTANCE;
				int32 MinIndex = 0;

				// Search through each node to find the one nearest to the portal.
				for (int32 NodeIndex = 0; NodeIndex < AssetObject->AreaNodes.Num(); NodeIndex++)
				{
					FTileNode& Node = AssetObject->AreaNodes[NodeIndex];
					float Distance = FVector::Distance(Portal.Location, Node.Location);

					if (Distance < MinDistance)
					{
						MinDistance = Distance;
						MinIndex = NodeIndex;
					}
				}

				// Save the index on the exits array.
				AssetObject->AreaExits.Add(MinIndex);
			}

			AssetPackage->MarkPackageDirty();

			// Get the package path relative to the local disk.
			FString FileName = FPackageName::LongPackageNameToFilename(
				AssetPackage->GetName(),
				FPackageName::GetAssetPackageExtension()
			);

			// Attempt to actually save the package and its contents. If the asset and package were
			// created as part of this function call, link them in the actor as well.
			if (UPackage::SavePackage(AssetPackage, AssetObject, *FileName, FSavePackageArgs()) && bNewAsset)
			{
				DataAsset = AssetObject;
			}
		}
	}
}

#endif
