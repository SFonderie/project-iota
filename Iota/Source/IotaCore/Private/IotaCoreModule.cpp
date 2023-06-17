// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "IotaCoreModule.h"
#include "Misc/CoreDelegates.h"
#include "Modules/ModuleManager.h"
#include "AbilitySystemGlobals.h"

void FIotaCoreModule::StartupModule()
{
	// The Ability System Globals object needs to be initialized near the end of the UEngine::Init method to avoid crashes.
	// FCoreDelegates::OnPostEngineInit broadcasts immediately following UEngine::Init, so it makes a good tie-in point.
	AbilitySystemDelegateHandle = FCoreDelegates::OnPostEngineInit.AddRaw(this, &FIotaCoreModule::InitializeAbilitySystem);
}

void FIotaCoreModule::ShutdownModule()
{
	// AddRaw means that we must remove the delegate bindings ourselves.
	FCoreDelegates::OnPostEngineInit.Remove(AbilitySystemDelegateHandle);
}

void FIotaCoreModule::InitializeAbilitySystem()
{
	UAbilitySystemGlobals::Get().InitGlobalData();
}

IMPLEMENT_PRIMARY_GAME_MODULE(FIotaCoreModule, IotaCore, "Iota");
