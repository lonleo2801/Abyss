// Copyright Epic Games, Inc. All Rights Reserved.

#include "CommonPreLoadScreen.h"
#include "Misc/App.h"
#include "Modules/ModuleManager.h"
#include "PreLoadScreenManager.h"

#define LOCTEXT_NAMESPACE "FCommonLoadingScreenModule"

class FCommonStartupLoadingScreenModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	bool IsGameModule() const override;

private:
	void OnPreLoadScreenManagerCleanUp();

	TSharedPtr<FCommonPreLoadScreen> PreLoadingScreen;
};


void FCommonStartupLoadingScreenModule::StartupModule()
{
	// No need to load these assets on dedicated servers.
	// Still want to load them in commandlets so cook catches them
	// 无需在专用服务器上加载这些资源。
	// 但您仍希望在命令行中加载它们，以便烹饪程序能够获取到它们。
	if (!IsRunningDedicatedServer())
	{
		PreLoadingScreen = MakeShared<FCommonPreLoadScreen>();
		PreLoadingScreen->Init();

		if (!GIsEditor && FApp::CanEverRender() && FPreLoadScreenManager::Get())
		{
			FPreLoadScreenManager::Get()->RegisterPreLoadScreen(PreLoadingScreen);
			FPreLoadScreenManager::Get()->OnPreLoadScreenManagerCleanUp.AddRaw(this, &FCommonStartupLoadingScreenModule::OnPreLoadScreenManagerCleanUp);
		}
	}
}

void FCommonStartupLoadingScreenModule::OnPreLoadScreenManagerCleanUp()
{
	//Once the PreLoadScreenManager is cleaning up, we can get rid of all our resources too
	// 当预加载屏幕管理器完成清理工作后，我们也可以一并清除所有资源了。
	PreLoadingScreen.Reset();
	ShutdownModule();
}

void FCommonStartupLoadingScreenModule::ShutdownModule()
{

}

bool FCommonStartupLoadingScreenModule::IsGameModule() const
{
	return true;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCommonStartupLoadingScreenModule, CommonStartupLoadingScreen)
