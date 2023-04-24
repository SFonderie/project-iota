#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "IotaAssetManager.generated.h"

/** Asset Manager that provides additional load functionality. */
UCLASS()
class IOTACORE_API UIotaAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:

	/** Ensures that the Ability System Globals object is properly initialized. */
	virtual void StartInitialLoading() override;
};
