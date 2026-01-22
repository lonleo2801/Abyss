#pragma once
 
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class AdvancedSteamSessions : public IModuleInterface
{
public:
	/** IModuleInterface 实现 */
	void StartupModule();
	void ShutdownModule();
};