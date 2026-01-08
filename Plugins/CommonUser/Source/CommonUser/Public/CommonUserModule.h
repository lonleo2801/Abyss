// Copyright Epic Games, Inc. All Rights Reserved.
// Finished.
#pragma once

#include "Modules/ModuleInterface.h"

class FCommonUserModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
