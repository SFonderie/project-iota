// Copyright Sydney Fonderie. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"

class IOTAGAME_API FIotaGameModule : public IModuleInterface
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
};
