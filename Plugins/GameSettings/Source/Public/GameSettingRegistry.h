// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameSetting.h"
#include "Templates/Casts.h"

#include "GameSettingRegistry.generated.h"

#define UE_API GAMESETTINGS_API

struct FGameplayTag;

//--------------------------------------
// UGameSettingRegistry
//--------------------------------------

class ULocalPlayer;
struct FGameSettingFilterState;

enum class EGameSettingChangeReason : uint8;

/**
 * 游戏设置注册器 用来连接具体的游戏设置和UI 承上启下
 */
UCLASS(MinimalAPI, Abstract, BlueprintType)
class UGameSettingRegistry : public UObject
{
	GENERATED_BODY()

public:
	// 定义 游戏设置发生改变的代理
	DECLARE_EVENT_TwoParams(UGameSettingRegistry, FOnSettingChanged, UGameSetting*, EGameSettingChangeReason);

	// 定义 设置编辑条件发生改变的代理
	DECLARE_EVENT_OneParam(UGameSettingRegistry, FOnSettingEditConditionChanged, UGameSetting*);

	// 游戏设置发生改变的代理
	FOnSettingChanged OnSettingChangedEvent;
	// 设置编辑条件发生改变的代理
	FOnSettingEditConditionChanged OnSettingEditConditionChangedEvent;

	// 设置被命名的行动事件
	DECLARE_EVENT_TwoParams(UGameSettingRegistry, FOnSettingNamedActionEvent, UGameSetting* /*Setting*/, FGameplayTag /*GameSettings_Action_Tag*/);
	FOnSettingNamedActionEvent OnSettingNamedActionEvent;

	/** Navigate to the child settings of the provided setting. */
	/** 跳转至所提供的设置的子设置页面。*/
	DECLARE_EVENT_OneParam(UGameSettingRegistry, FOnExecuteNavigation, UGameSetting* /*Setting*/);
	FOnExecuteNavigation OnExecuteNavigationEvent;

public:
	// 构造函数 无作用
	UE_API UGameSettingRegistry();

	// 初始化 设置本地玩家类
	// 走这里初始化的ULyraSettingScreen::CreateRegistry()
	UE_API void Initialize(ULocalPlayer* InLocalPlayer);

	// 重新生成
	UE_API virtual void Regenerate();

	// 是否完成初始
	UE_API virtual bool IsFinishedInitializing() const;

	// 保存变化 由UGameSettingScreen::ApplyChanges调用
	UE_API virtual void SaveChanges();

	// 过滤游戏设置
	UE_API void GetSettingsForFilter(const FGameSettingFilterState& FilterState, TArray<UGameSetting*>& InOutSettings);

	// 根据名字找到对应的游戏设置
	UE_API UGameSetting* FindSettingByDevName(const FName& SettingDevName);

	// 带有检测的获取游戏设置
	template<typename T = UGameSetting>
	T* FindSettingByDevNameChecked(const FName& SettingDevName)
	{
		T* Setting = Cast<T>(FindSettingByDevName(SettingDevName));
		check(Setting);
		return Setting;
	}

protected:
	
	// 实际内部初始化 虚函数
	UE_API virtual void OnInitialize(ULocalPlayer* InLocalPlayer) PURE_VIRTUAL(, )

	// 当设置应用时
	virtual void OnSettingApplied(UGameSetting* Setting) { }

	// 注册设置
	UE_API void RegisterSetting(UGameSetting* InSetting);
	// 注册设置 内部执行
	UE_API void RegisterInnerSettings(UGameSetting* InSetting);

	// Internal event handlers.
	// 内部事件处理程序。

	// 处理设置改变
	UE_API void HandleSettingChanged(UGameSetting* Setting, EGameSettingChangeReason Reason);
	// 处理设置应用
	UE_API void HandleSettingApplied(UGameSetting* Setting);
	// 处理设置编辑条件发生改变
	UE_API void HandleSettingEditConditionsChanged(UGameSetting* Setting);
	// 处理动作事件
	UE_API void HandleSettingNamedAction(UGameSetting* Setting, FGameplayTag GameSettings_Action_Tag);
	// 处理导航事件
	UE_API void HandleSettingNavigation(UGameSetting* Setting);

	// 顶层的设置 只注册顶层的 不需要递归注册子项!
	UPROPERTY(Transient)
	TArray<TObjectPtr<UGameSetting>> TopLevelSettings;

	// 已经注册的设置
	UPROPERTY(Transient)
	TArray<TObjectPtr<UGameSetting>> RegisteredSettings;

	// 指向的本地玩家类
	UPROPERTY(Transient)
	TObjectPtr<ULocalPlayer> OwningLocalPlayer;
};

#undef UE_API
