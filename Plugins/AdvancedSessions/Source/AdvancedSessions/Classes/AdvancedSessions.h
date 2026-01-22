#pragma once
 
#include "Modules/ModuleManager.h"

class AdvancedSessions : public IModuleInterface
{
public:
	/** IModuleInterface 实现 */
	void StartupModule();
	void ShutdownModule();
};