// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGen/TileGenAsyncAction.h"
#include "Engine/AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileGenAsyncAction)

UTileGenAsyncAction* UTileGenAsyncAction::GenerateLevel(const UObject* WorldContext, const FTileGenParams& Params)
{
	UTileGenAsyncAction* NewAction = NewObject<UTileGenAsyncAction>();
	NewAction->RegisterWithGameInstance(WorldContext);
	NewAction->Params = Params;
	return NewAction;
}

void UTileGenAsyncAction::Activate()
{

}

void UTileGenAsyncAction::Cancel()
{
	Super::Cancel();
}