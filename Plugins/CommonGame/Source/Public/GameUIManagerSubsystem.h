// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/SoftObjectPtr.h"

#include "GameUIManagerSubsystem.generated.h"

#define UE_API COMMONGAME_API

class FSubsystemCollectionBase;
class UCommonLocalPlayer;
class UGameUIPolicy;
class UObject;

/**
 * This manager is intended to be replaced by whatever your game needs to
 * actually create, so this class is abstract to prevent it from being created.
 *
 * 这个管理器旨在根据您的游戏实际需求进行替换，因此这个类是抽象类，以防止其被创建。
 * 
 * If you just need the basic functionality you will start by sublcassing this
 * subsystem in your own game.
 *
 * 如果您只需要基本功能，那么您只需在自己的游戏中继承此子系统即可。
 * 
 */
UCLASS(MinimalAPI, Abstract, config = Game)
class UGameUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UGameUIManagerSubsystem() { }

	// 创建策略
	UE_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 回收策略
	UE_API virtual void Deinitialize() override;

	// 如果开发者没有自定义才需要创建该子系统
	UE_API virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	// 获取当前策略
	const UGameUIPolicy* GetCurrentUIPolicy() const { return CurrentPolicy; }

	// 获取当前策略
	UGameUIPolicy* GetCurrentUIPolicy() { return CurrentPolicy; }

	// 转发
	UE_API virtual void NotifyPlayerAdded(UCommonLocalPlayer* LocalPlayer);

	// 转发
	UE_API virtual void NotifyPlayerRemoved(UCommonLocalPlayer* LocalPlayer);

	// 转发
	UE_API virtual void NotifyPlayerDestroyed(UCommonLocalPlayer* LocalPlayer);

protected:
	// 切换策略
	UE_API void SwitchToPolicy(UGameUIPolicy* InPolicy);

private:
	// 持有
	UPROPERTY(Transient)
	TObjectPtr<UGameUIPolicy> CurrentPolicy = nullptr;

	// 配置
	UPROPERTY(config, EditAnywhere)
	TSoftClassPtr<UGameUIPolicy> DefaultUIPolicyClass;
};

#undef UE_API
