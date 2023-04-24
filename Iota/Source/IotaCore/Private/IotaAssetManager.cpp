#include "IotaAssetManager.h"
#include "AbilitySystemGlobals.h"

void UIotaAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// Initialize the Ability System Globals data.
	UAbilitySystemGlobals::Get().InitGlobalData();
}
