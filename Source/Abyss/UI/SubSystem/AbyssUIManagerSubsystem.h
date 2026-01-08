// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "GameUIManagerSubsystem.h"

#include "AbyssUIManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssUIManagerSubsystem : public UGameUIManagerSubsystem
{
	GENERATED_BODY()
public:

	UAbyssUIManagerSubsystem();

	// 绑定Tick
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 移除Tick
	virtual void Deinitialize() override;

private:
	// Tick
	bool Tick(float DeltaTime);
	
	// 更改UI布局的可实现
	void SyncRootLayoutVisibilityToShowHUD();
	
	FTSTicker::FDelegateHandle TickHandle;
};
