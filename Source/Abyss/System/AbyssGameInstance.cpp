// Copyright (c) 2025 Leon Lee


#include "AbyssGameInstance.h"

UAbyssGameInstance::UAbyssGameInstance(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

void UAbyssGameInstance::Init()
{
	Super::Init();

	// 这里可以初始化你的项目逻辑（数据管理、UI系统等）
	UE_LOG(LogTemp, Log, TEXT("UAbyssGameInstance::Init()"));
}

void UAbyssGameInstance::Shutdown()
{
	Super::Shutdown();
	UE_LOG(LogTemp, Log, TEXT("UAbyssGameInstance::Shutdown()"));
}
