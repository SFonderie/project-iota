#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

/** Game module class representing IotaCore. */
class FIotaCoreModule : public IModuleInterface
{
	/** Marks IotaCore as a gameplay module. Always returns true. */
	virtual bool IsGameModule() const override
	{
		return true;
	}
};
