// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "UObject/WeakInterfacePtr.h"

#include "LoadingScreenManager.generated.h"

#define UE_API COMMONLOADINGSCREEN_API

template <typename InterfaceType> class TScriptInterface;

class FSubsystemCollectionBase;
class IInputProcessor;
class ILoadingProcessInterface;
class SWidget;
class UObject;
class UWorld;
struct FFrame;
struct FWorldContext;

/**
 * Handles showing/hiding the loading screen
 * 控制显示/隐藏加载界面
 */
UCLASS(MinimalAPI)
class ULoadingScreenManager : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	//~USubsystem interface
	// 绑定地图的变动
	UE_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// 移除加载屏幕的功能 
	UE_API virtual void Deinitialize() override;
	// 客户端才创建
	UE_API virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~End of USubsystem interface

	//~FTickableObjectBase interface
	// 每帧检测是否要显示加载屏幕
	UE_API virtual void Tick(float DeltaTime) override;
	// 决定Tick的开关类型
	UE_API virtual ETickableTickType GetTickableTickType() const override;
	// 补充是否需要显示加载屏幕
	UE_API virtual bool IsTickable() const override;
	// ID
	UE_API virtual TStatId GetStatId() const override;
	// 应用的世界
	UE_API virtual UWorld* GetTickableGameObjectWorld() const override;
	//~End of FTickableObjectBase interface

	// 暴露给蓝图询问
	UFUNCTION(BlueprintCallable, Category=LoadingScreen)
	FString GetDebugReasonForShowingOrHidingLoadingScreen() const
	{
		return DebugReasonForShowingOrHidingLoadingScreen;
	}

	/** Returns True when the loading screen is currently being shown */
	/** 当加载界面正在显示时返回 True */
	bool GetLoadingScreenDisplayStatus() const
	{
		return bCurrentlyShowingLoadingScreen;
	}

	/** Called when the loading screen visibility changes  */
	/** 当加载界面的可见性发生变化时调用 */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoadingScreenVisibilityChangedDelegate, bool);
	FORCEINLINE FOnLoadingScreenVisibilityChangedDelegate& OnLoadingScreenVisibilityChangedDelegate() { return LoadingScreenVisibilityChanged; }

	// 由具体的接口实例进行生命周期的注册
	UE_API void RegisterLoadingProcessor(TScriptInterface<ILoadingProcessInterface> Interface);
	UE_API void UnregisterLoadingProcessor(TScriptInterface<ILoadingProcessInterface> Interface);
	
private:
	// 触发更新
	UE_API void HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName);
	// 触发更新
	UE_API void HandlePostLoadMap(UWorld* World);

	/** Determines if we should show or hide the loading screen. Called every frame. */
	/** 用于确定是否应显示或隐藏加载界面。每帧都会调用此函数。*/
	UE_API void UpdateLoadingScreen();

	/** Returns true if we need to be showing the loading screen. */
	/** 若需要显示加载界面，则返回 true 。*/
	UE_API bool CheckForAnyNeedToShowLoadingScreen();

	/** Returns true if we want to be showing the loading screen (if we need to or are artificially forcing it on for other reasons). */
	/** 若我们希望显示加载界面，则返回 true（这意味着我们需要显示该界面，或者是因为其他人为原因而强制显示）。*/
	UE_API bool ShouldShowLoadingScreen();

	/** Returns true if we are in the initial loading flow before this screen should be used */
	/** 如果我们正处于初始加载流程中，且在此屏幕投入使用之前，返回 true */
	UE_API bool IsShowingInitialLoadingScreen() const;

	/** Shows the loading screen. Sets up the loading screen widget on the viewport */
	/** 显示加载界面。在视口上设置加载界面组件 */
	UE_API void ShowLoadingScreen();

	/** Hides the loading screen. The loading screen widget will be destroyed */
	/** 隐藏加载界面。加载界面的组件将会被销毁 */
	UE_API void HideLoadingScreen();

	/** Removes the widget from the viewport */
	/** 将该控件从视口中移除 */
	UE_API void RemoveWidgetFromViewport();

	/** Prevents input from being used in-game while the loading screen is visible */
	/** 在加载界面显示期间，防止输入内容被用于游戏之中 */
	UE_API void StartBlockingInput();

	/** Resumes in-game input, if blocking */
	/** 若存在阻塞情况，则恢复游戏中的输入操作 */
	UE_API void StopBlockingInput();

	// 变更由于切换加载屏幕的性能影响
	UE_API void ChangePerformanceSettings(bool bEnabingLoadingScreen);

private:
	/** Delegate broadcast when the loading screen visibility changes */
	/** 当加载界面的可见性发生变化时，执行广播操作 */
	FOnLoadingScreenVisibilityChangedDelegate LoadingScreenVisibilityChanged;

	/** A reference to the loading screen widget we are displaying (if any) */
	/** 指向我们正在显示的加载屏幕组件的引用（如果有的话） */
	TSharedPtr<SWidget> LoadingScreenWidget;

	/** Input processor to eat all input while the loading screen is shown */
	/** 用于在加载界面显示期间接收所有输入的输入处理器 */
	TSharedPtr<IInputProcessor> InputPreProcessor;

	/** External loading processors, components maybe actors that delay the loading. */
	/** 外部加载处理器、组件可能包括那些会延迟加载过程的执行者。*/
	TArray<TWeakInterfacePtr<ILoadingProcessInterface>> ExternalLoadingProcessors;

	/** The reason why the loading screen is up (or not) */
	/** 加载界面是否显示（或未显示）的原因 */
	FString DebugReasonForShowingOrHidingLoadingScreen;

	/** The time when we started showing the loading screen */
	/** 我们开始显示加载界面的时间 */
	double TimeLoadingScreenShown = 0.0;

	/** The time the loading screen most recently wanted to be dismissed (might still be up due to a min display duration requirement) **/
	/** 加载界面最后一次想要被关闭的时间（由于最小显示时长要求，该界面可能仍处于显示状态） **/
	double TimeLoadingScreenLastDismissed = -1.0;

	/** The time until the next log for why the loading screen is still up */
	/** 下一次记录加载屏幕仍处于显示状态的原因所需的时间 */
	double TimeUntilNextLogHeartbeatSeconds = 0.0;

	/** True when we are between PreLoadMap and PostLoadMap */
	/** 当我们处于“预加载地图”与“后加载地图”之间时为真 */
	bool bCurrentlyInLoadMap = false;

	/** True when the loading screen is currently being shown */
	/** 当加载界面正在显示时为真 */
	bool bCurrentlyShowingLoadingScreen = false;
};

#undef UE_API
