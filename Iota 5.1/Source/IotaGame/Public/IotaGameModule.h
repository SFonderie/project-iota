// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

/** Game module class representing IotaCore. */
class FIotaGameModule : public IModuleInterface
{

public:

	/** Invoked to initialize IotaGame during module startup. */
	virtual void StartupModule() override;

	/** Invoked to unload IotaGame safely during module shutdown. */
	virtual void ShutdownModule() override;

	/** Marks IotaGame as a gameplay module. Always returns true. */
	virtual bool IsGameModule() const override
	{
		return true;
	}

private:

	/** Invoked once to initialize the Ability System Globals data. */
	void InitializeAbilitySystem();

	/** Handle for InitializeAbilitySystem on FCoreDelegates::OnPostEngineInit. */
	FDelegateHandle AbilitySystemDelegateHandle;
};
