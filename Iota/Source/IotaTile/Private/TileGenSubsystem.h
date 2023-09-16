// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TileGenSubsystem.generated.h"

class UTileGenAsyncAction;

/** Tracks and registers level generation async actions. */
UCLASS(NotBlueprintType)
class UTileGenSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	/** Invoked during shutdown to cancel and release any remaining async actions. */
	virtual void Deinitialize() override;

	/**
	 * Adds a new generator to the local list and returns an ID for that generator.
	 *
	 * @param AsyncAction Tile generation async action to register with the subsystem.
	 * @param OutID ID number for the newly-registered action.
	 */
	void RegisterGenerator(UTileGenAsyncAction* AsyncAction, int32& OutID);

private:

	/** Generation async actions created on this world. */
	TArray<TWeakObjectPtr<UTileGenAsyncAction>> GeneratorActions;
};
