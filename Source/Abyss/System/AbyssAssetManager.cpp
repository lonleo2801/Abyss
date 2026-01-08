// Copyright (c) 2025 Leon Lee


#include "AbyssAssetManager.h"

#include "DataRegistrySubsystem.h"

UAbyssAssetManager& UAbyssAssetManager::Get()
{
	check(GEngine);

	UAbyssAssetManager* AuraAssetManager = Cast<UAbyssAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;
}

void UAbyssAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	//用于项目启动时的一次性资源或系统初始化。

	
	// 如果希望在游戏启动阶段就把某些 Registry 全加载，可以
	if (UDataRegistrySubsystem* RegistrySubsystem = GEngine->GetEngineSubsystem<UDataRegistrySubsystem>())
	{
		// 强制初始化所有注册表（避免第一次访问时卡顿）
		RegistrySubsystem->InitializeAllRegistries();
	}
	
}
