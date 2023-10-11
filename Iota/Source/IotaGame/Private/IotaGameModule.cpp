// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "IotaGameModule.h"
#include "Misc/CoreDelegates.h"
#include "Modules/ModuleManager.h"
#include "AbilitySystemGlobals.h"

void FIotaGameModule::StartupModule()
{
	// The Ability System Globals object needs to be initialized near the end of the UEngine::Init method to avoid crashes.
	// FCoreDelegates::OnPostEngineInit broadcasts immediately following UEngine::Init, so it makes a good tie-in point.
	AbilitySystemDelegateHandle = FCoreDelegates::OnPostEngineInit.AddRaw(this, &FIotaGameModule::InitializeAbilitySystem);
}

void FIotaGameModule::ShutdownModule()
{
	// AddRaw means that we must remove the delegate bindings ourselves.
	FCoreDelegates::OnPostEngineInit.Remove(AbilitySystemDelegateHandle);
}

void FIotaGameModule::InitializeAbilitySystem()
{
	UAbilitySystemGlobals::Get().InitGlobalData();
}

IMPLEMENT_PRIMARY_GAME_MODULE(FIotaGameModule, IotaGame, "Iota");
