// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonActivatableWidget.h"
#include "GameSettingRegistry.h"
#include "GameSettingRegistryChangeTracker.h"

#include "GameSettingScreen.generated.h"

#define UE_API GAMESETTINGS_API

class UGameSetting;
class UGameSettingCollection;
class UGameSettingPanel;
class UObject;
class UWidget;
struct FFrame;

enum class EGameSettingChangeReason : uint8;

/**
 * 游戏设置的主屏幕
 * 1. 持有游戏设置注册器
 * 2. 持有游戏设置变动追踪器
 * 3. 提供若干蓝图接口,如创建游戏设置器,应用或取消游戏设置,导航等逻辑
 */
UCLASS(MinimalAPI, Abstract, meta = (Category = "Settings", DisableNativeTick))
class UGameSettingScreen : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	
protected:
	// 无
	UE_API virtual void NativeOnInitialized() override;
	// 初始化游戏设置注册器的追踪
	UE_API virtual void NativeOnActivated() override;
	// 无
	UE_API virtual void NativeOnDeactivated() override;

	// 重写一下聚焦对象 如果蓝图设置就走蓝图的 如果蓝图没就走设置的细节面板
	UE_API virtual UWidget* NativeGetDesiredFocusTarget() const override;

	// 导航设置 由蓝图调用 接收由TabList在注册Tag或点击Tag时调用!
	UFUNCTION(BlueprintCallable)
	UE_API void NavigateToSetting(FName SettingDevName);

	// 导航到对应的设置
	UFUNCTION(BlueprintCallable)
	UE_API void NavigateToSettings(const TArray<FName>& SettingDevNames);

	// 设置变动触发的回调
	UFUNCTION(BlueprintNativeEvent)
	UE_API void OnSettingsDirtyStateChanged(bool bSettingsDirty);
	virtual void OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty) { }

	// 尝试弹出导航栈
	UFUNCTION(BlueprintCallable)
	UE_API bool AttemptToPopNavigation();

	// 暴露给蓝图 获取对应的游戏设置集合!首次使用时创建,调用的位置是蓝图的注册Tab
	// W_LyraSettingScreen->Construct->RegitterTopLevelTab->GetSettingCollection
	UFUNCTION(BlueprintCallable)
	UE_API UGameSettingCollection* GetSettingCollection(FName SettingDevName, bool& HasAnySettings); 

protected:
	// 创建游戏设置注册器
	UE_API virtual UGameSettingRegistry* CreateRegistry() PURE_VIRTUAL(, return nullptr;);

	// 获取游戏设置注册器
	template <typename GameSettingRegistryT = UGameSettingRegistry>
	GameSettingRegistryT* GetRegistry() const { return Cast<GameSettingRegistryT>(const_cast<UGameSettingScreen*>(this)->GetOrCreateRegistry()); }

	// 取消变化
	UFUNCTION(BlueprintCallable)
	UE_API virtual void CancelChanges();

	// 应用变化
	UFUNCTION(BlueprintCallable)
	UE_API virtual void ApplyChanges();

	// 是否有设置变动
	UFUNCTION(BlueprintCallable)
	bool HaveSettingsBeenChanged() const { return ChangeTracker.HaveSettingsBeenChanged(); }

	// 清理脏的状态
	UE_API void ClearDirtyState();

	// 在GetOrCreateRegistry中进行绑定
	UE_API void HandleSettingChanged(UGameSetting* Setting, EGameSettingChangeReason Reason);

	FGameSettingRegistryChangeTracker ChangeTracker;

private:
	// 内部获取或创建游戏设置注册器
	UE_API UGameSettingRegistry* GetOrCreateRegistry();

private:
	// Bound Widgets
	// 固定的控件
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UGameSettingPanel> Settings_Panel;

	// 由它持有 ok  没有问题
	UPROPERTY(Transient)
	mutable TObjectPtr<UGameSettingRegistry> Registry;
};

#undef UE_API
