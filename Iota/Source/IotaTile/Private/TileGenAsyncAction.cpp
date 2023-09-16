// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGenAsyncAction.h"
#include "TileGenDoor.h"
#include "TileGenSubsystem.h"
#include "TileGenWorker.h"
#include "Engine/Engine.h"
#include "Engine/AssetManager.h"
#include "Engine/LevelStreamingDynamic.h"
#include "TileData/TileDataAsset.h"
#include "TileData/TilePlan.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileGenAsyncAction)

UTileGenAsyncAction* UTileGenAsyncAction::GenerateTileLevel(UObject* WorldContextObject, const FTileGenParams& Parameters)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		UTileGenAsyncAction* NewAction = NewObject<UTileGenAsyncAction>();
		NewAction->RegisterWithGameInstance(World->GetGameInstance());
		NewAction->Params = Parameters;
		NewAction->World = World;

		// Registration with the world ensures the action is cancelled on world shutdown.
		// The returned ID number also ensures world-unique names for loaded tile levels.
		UTileGenSubsystem* Subsystem = World->GetSubsystem<UTileGenSubsystem>();
		Subsystem->RegisterGenerator(NewAction, NewAction->SubsystemID);
		return NewAction;
	}

	return nullptr;
}

void UTileGenAsyncAction::Activate()
{
	if (!bHasActivated)
	{
		NotifyProcessStart();
		bHasActivated = true;
	}
}

void UTileGenAsyncAction::Cancel()
{
	Super::Cancel();

	if (DataAssetHandle.IsValid())
	{
		DataAssetHandle->CancelHandle();
		DataAssetHandle.Reset();
	}

	if (GenerationWorker.IsValid())
	{
		GenerationWorker.Reset();
	}

	for (ULevelStreaming* Stream : TileStreams)
	{
		Stream->SetIsRequestingUnloadAndRemoval(true);
	}

	TileStreams.Empty();
}

void UTileGenAsyncAction::NotifyProcessStart()
{
	UAssetManager& AssetManager = UAssetManager::Get();

	// Load all tile data asset information into a list so it can be filtered.
	TArray<FAssetData> TileAssetData;
	AssetManager.GetPrimaryAssetDataList(UTileDataAsset::StaticClass()->GetFName(), TileAssetData);

	// Filter the asset data tags for the target tileset.
	// If found, add the asset data to the tile ID list.
	for (const FAssetData& AssetData : TileAssetData)
	{
		FGameplayTag AssetDataTag;
		FString AssetDataRawString;

		if (AssetData.GetTagValue(GET_MEMBER_NAME_CHECKED(UTileDataAsset, Tileset), AssetDataRawString))
		{
			AssetDataTag.FromExportString(AssetDataRawString);

			if (AssetDataTag == Params.Tileset)
			{
				TileAssetList.Emplace(AssetData.GetPrimaryAssetId());
			}
		}
	}

	// First asynchronous phase: executing is passed to the streamable manager to load in requested
	// tile data assets. If no loading is necessary, the delegate will fire immediately.
	FStreamableDelegate Callback = FStreamableDelegate::CreateUObject(this, &UTileGenAsyncAction::NotifyAssetsLoaded);
	DataAssetHandle = AssetManager.LoadPrimaryAssets(TileAssetList, TArray<FName>(), Callback);
}

void UTileGenAsyncAction::NotifyAssetsLoaded()
{
	// Create the worker, which actually handles most of the generation. 
	// See the Tile Gen Worker class for the actual generation code.
	GenerationWorker = MakeShared<FTileGenWorker>(Params, TileAssetList);

	// Bind a callback to the generation worker and actually start it up.
	if (GenerationWorker.IsValid())
	{
		GenerationWorker->OnExit.BindUObject(this, &UTileGenAsyncAction::NotifyWorkerComplete);
		GenerationWorker->Start();
	}

	// Creating the worker thread copies all loaded assets into a thread-local cache.
	// The assets can therefore be unloaded since they have served their purpose.
	if (DataAssetHandle.IsValid())
	{
		DataAssetHandle.Reset();
	}

	// Loaded primary assets must also be manually unloaded.
	UAssetManager& AssetManager = UAssetManager::Get();
	AssetManager.UnloadPrimaryAssets(TileAssetList);
}

void UTileGenAsyncAction::NotifyWorkerComplete()
{
	if (GenerationWorker.IsValid() && GenerationWorker->IsComplete())
	{
		TArray<FTilePlan> TilePlans;
		GenerationWorker->GetPlan(TilePlans);
		int32 Count = 0;

		for (const FTilePlan& Plan : TilePlans)
		{
			// Generate a unique level name using the world ID number and the tile index. Streamed
			// levels are client-side, so providing a name override ensures that loaded actors are
			// properly loaded on the server.
			FString NameOverride = FString::Printf(TEXT("TileLevel_%i_Tile_%i"), SubsystemID, Count);

			// Extract local values from the current tile plan.
			FTransform TileTransform(Plan.Rotation, Plan.Position);
			FString TilePackage = Plan.Level.GetLongPackageName();

			// Condense the streaming parameters.
			ULevelStreamingDynamic::FLoadLevelInstanceParams StreamParams(World, TilePackage, TileTransform);
			StreamParams.OptionalLevelNameOverride = &NameOverride;
			bool bSuccess = false;

			// Creates a new dynamic level stream and returns the result.
			ULevelStreaming* NewStream = ULevelStreamingDynamic::LoadLevelInstance(StreamParams, bSuccess);

			if (NewStream && bSuccess)
			{
				TileStreams.Emplace(NewStream);
				Count++;
			}
		}

		GenerateDoors();

		OnGenerationComplete.Broadcast();
		return;
	}

	OnGenerationFailure.Broadcast();
}

void UTileGenAsyncAction::GenerateDoors()
{
	UAssetManager& AssetManager = UAssetManager::Get();

	TArray<FAssetData> DoorAssetData;
	AssetManager.GetPrimaryAssetDataList(ATileGenDoor::StaticClass()->GetFName(), DoorAssetData);

	TMultiMap<FIntPoint, UClass*> DoorClasses;

	for (const FAssetData& AssetData : DoorAssetData)
	{
		TSubclassOf<ATileGenDoor> DoorClass = AssetManager.GetPrimaryAssetObjectClass<ATileGenDoor>(AssetData.GetPrimaryAssetId());
		ATileGenDoor* DoorObject = DoorClass.GetDefaultObject();

		if (DoorObject && DoorObject->Tilesets.HasTag(Params.Tileset))
		{
			DoorClasses.Emplace(DoorObject->DoorSize, DoorClass);
		}
	}

	TArray<FTilePortal> TilePortals, TruePortals, FakePortals;
	GenerationWorker->GetPlan(TilePortals);

	// Extremely hacky way to figure out which portals are fake (not connected to another tile) or
	// true (connected to another tile) via brute force search. Future versions should determine
	// portal status during generation and export it out.
	for (FTilePortal& Portal : TilePortals)
	{
		bool bFake = true;

		// Search the list of fake portals for a location match. If one is found, the portal is
		// a legitimate portal and thus can be added to the true list.
		for (int32 Fake = 0; Fake < FakePortals.Num(); Fake++)
		{
			if (FVector::PointsAreSame(FakePortals[Fake].Location, Portal.Location))
			{
				TruePortals.Emplace(Portal);
				FakePortals.RemoveAtSwap(Fake);
				bFake = false;
				break;
			}
		}

		if (bFake)
		{
			FakePortals.Emplace(Portal);
		}
	}

	for (const FTilePortal& Portal : TruePortals)
	{
		TArray<UClass*> Doors;
		DoorClasses.MultiFind(Portal.PlaneSize, Doors);
		UClass* RandomDoor = Doors[FMath::RandRange(0, Doors.Num() - 1)];

		FVector Position = Portal.Location - FVector(0, 0, Portal.PlaneSize.Y * 50);
		FRotator Rotation = FRotationMatrix::MakeFromX(Portal.Direction).Rotator();

		DoorActors.Add(World->SpawnActorAbsolute<ATileGenDoor>(RandomDoor, FTransform(Rotation, Position)));
	}

	for (const FTilePortal& Portal : FakePortals)
	{
		TArray<UClass*> Doors;
		DoorClasses.MultiFind(Portal.PlaneSize, Doors);
		UClass* RandomDoor = Doors[FMath::RandRange(0, Doors.Num() - 1)];

		FVector Position = Portal.Location - FVector(0, 0, Portal.PlaneSize.Y * 50);
		FRotator Rotation = FRotationMatrix::MakeFromX(Portal.Direction).Rotator();

		ATileGenDoor* FakeDoor = World->SpawnActorAbsolute<ATileGenDoor>(RandomDoor, FTransform(Rotation, Position));
		FakeDoor->bIsLocked = true;

		DoorActors.Add(FakeDoor);
	}
}
