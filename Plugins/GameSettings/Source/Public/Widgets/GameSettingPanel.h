// Copyright Epic Games, Inc. All Rights Reserved.
// Finished.
#pragma once

#include "CommonUserWidget.h"
#include "Containers/Ticker.h"
#include "GameSettingFilterState.h"
#include "GameplayTagContainer.h"
#include "Misc/ExpressionParserTypesFwd.h"

#include "GameSettingPanel.generated.h"

#define UE_API GAMESETTINGS_API

class UGameSetting;
class UGameSettingDetailView;
class UGameSettingListView;
class UGameSettingRegistry;
class UObject;
struct FFocusEvent;
struct FGeometry;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnFocusedSettingChanged, UGameSetting*)

// 设置的具体面板
UCLASS(MinimalAPI, Abstract)
class UGameSettingPanel : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	// 构造函数 设置可以被聚焦
	UE_API UGameSettingPanel();

	// 绑定监听的设置条目发生变化
	UE_API virtual void NativeOnInitialized() override;

	// 注册监听游戏设置注册器的三种类型事件
	UE_API virtual void NativeConstruct() override;

	// 停用监听游戏设置注册器的三种类型事件
	UE_API virtual void NativeDestruct() override;

	// Focus transitioning to subwidgets for the gamepad
	// 转向游戏手柄的子组件进行重点展示
	UE_API virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	/**  设置游戏注册器 并注册其对应的事件 并刷新游戏设置的面板细节 */
	UE_API void SetRegistry(UGameSettingRegistry* InRegistry);

	/** Sets the filter for this panel, restricting which settings are available currently. */
	/** 为该面板设置筛选条件，以限制当前可使用的设置范围。*/
	// 其实就是外部Screen导航过来的入口
	UE_API void SetFilterState(const FGameSettingFilterState& InFilterState, bool bClearNavigationStack = true);

	/** Gets the currently visible and available settings based on the filter state. */
	/** 根据过滤状态获取当前可见且可用的设置。*/
	TArray<UGameSetting*> GetVisibleSettings() const { return VisibleSettings; }

	/** Can we pop the current navigation stack */
	/** 我们能否弹出当前的导航栈？ */
	UE_API bool CanPopNavigationStack() const;

	/** Pop the navigation stack */
	/** 弹出导航栈 */
	UE_API void PopNavigationStack();

	/**
	 * Gets the set of settings that are potentially available on this screen.
	 * MAY CONTAIN INVISIBLE SETTINGS.
	 * DOES NOT INCLUDED NESTED PAGES.
	 *
	 * 获取此屏幕上可能存在的设置列表。
	 * 可能包含不可见的设置。
	 * 不包含嵌套页面。
	 */
	UE_API TArray<UGameSetting*> GetSettingsWeCanResetToDefault() const;

	// 选中某项
	UE_API void SelectSetting(const FName& SettingDevName);
	// 获取当前选中的游戏设置
	UE_API UGameSetting* GetSelectedSetting() const;

	// 刷新设置列表
	UE_API void RefreshSettingsList();

	// 聚焦的设置项目发生改变后触发的代理
	FOnFocusedSettingChanged OnFocusedSettingChanged;

protected:
	// 注册 游戏注册器的事件
	UE_API void RegisterRegistryEvents();
	// 停用 游戏注册器的事件
	UE_API void UnregisterRegistryEvents();

	// 悬停时 更新细节面板
	UE_API void HandleSettingItemHoveredChanged(UObject* Item, bool bHovered);
	// 选中时 更新细节面板
	UE_API void HandleSettingItemSelectionChanged(UObject* Item);
	// 填充细节面板
	UE_API void FillSettingDetails(UGameSetting* InSetting);

	// 处理游戏设置注册器的操作事件
	UE_API void HandleSettingNamedAction(UGameSetting* Setting, FGameplayTag GameSettings_Action_Tag);
	// 处理游戏设置注册器的导航事件
	UE_API void HandleSettingNavigation(UGameSetting* Setting);
	// 处理游戏设置注册器的编辑检索条件发生变化的事件
	UE_API void HandleSettingEditConditionsChanged(UGameSetting* Setting);

private:

	// 对应的游戏设置注册器
	UPROPERTY(Transient)
	TObjectPtr<UGameSettingRegistry> Registry;

	// 当前可见的游戏设置
	UPROPERTY(Transient)
	TArray<TObjectPtr<UGameSetting>> VisibleSettings;

	// 上次悬停或选中的游戏设置
	UPROPERTY(Transient)
	TObjectPtr<UGameSetting> LastHoveredOrSelectedSetting;

	// 过滤器
	UPROPERTY(Transient)
	FGameSettingFilterState FilterState;

	// 导航的栈
	UPROPERTY(Transient)
	TArray<FGameSettingFilterState> FilterNavigationStack;

	// 在刷新之后需要进行选中的游戏设置项的名称
	FName DesiredSelectionPostRefresh;

	// 在刷新之后是否需要调整选中的游戏设置项
	bool bAdjustListViewPostRefresh = true;

private:
	// Bound Widgets
	// 固定的组件
	// 设置的条目容器
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UGameSettingListView> ListView_Settings;

	// 具体的设置细节
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UGameSettingDetailView> Details_Settings;

private:
	// 命名设置操作以执行特殊操作
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnExecuteNamedActionBP, UGameSetting*, Setting, FGameplayTag, Action);
	UPROPERTY(BlueprintAssignable, Category = Events, meta = (DisplayName = "On Execute Named Action"))
	FOnExecuteNamedActionBP BP_OnExecuteNamedAction;

private:
	// 用于刷新设置条目的句柄
	FTSTicker::FDelegateHandle RefreshHandle;
};

#undef UE_API
