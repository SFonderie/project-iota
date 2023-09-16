// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGenSubsystem.h"
#include "TileGenAsyncAction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileGenSubsystem)

void UTileGenSubsystem::Deinitialize()
{
	for (TWeakObjectPtr<UTileGenAsyncAction>& ActionPointer : GeneratorActions)
	{
		if (UTileGenAsyncAction* Action = ActionPointer.Get())
		{
			Action->Cancel();
		}
	}
}

void UTileGenSubsystem::RegisterGenerator(UTileGenAsyncAction* AsyncAction, int32& OutID)
{
	OutID = GeneratorActions.Num();
	GeneratorActions.Emplace(AsyncAction);
}
