// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonActivatableWidget.h"
#include "CommonUIExtensions.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameplayTagContainer.h"
#include "Widgets/CommonActivatableWidgetContainer.h" // IWYU pragma: keep

#include "PrimaryGameLayout.generated.h"

#define UE_API COMMONGAME_API

class APlayerController;
class UClass;
class UCommonActivatableWidgetContainerBase;
class ULocalPlayer;
class UObject;
struct FFrame;

/**
 * The state of an async load operation for the UI.
 * 用户界面的异步加载操作的状态。
 */
enum class EAsyncWidgetLayerState : uint8
{
	Canceled,
	Initialize,
	AfterPush
};

/**
 * The primary game UI layout of your game.  This widget class represents how to layout, push and display all layers
 * of the UI for a single player.  Each player in a split-screen game will receive their own primary game layout.
 *
 * * 您游戏的主要用户界面布局。此组件类描述了如何为单个玩家布局、推送和显示整个用户界面的各个层。在分屏游戏中，每个玩家都将拥有自己的主要游戏布局。
 */
UCLASS(MinimalAPI, Abstract, meta = (DisableNativeTick))
class UPrimaryGameLayout : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	// 获取对应的根布局 从世界上下文,玩家控制器或本地玩家对象
	static UE_API UPrimaryGameLayout* GetPrimaryGameLayoutForPrimaryPlayer(const UObject* WorldContextObject);
	static UE_API UPrimaryGameLayout* GetPrimaryGameLayout(APlayerController* PlayerController);
	static UE_API UPrimaryGameLayout* GetPrimaryGameLayout(ULocalPlayer* LocalPlayer);

public:
	// 构造函数无作用
	UE_API UPrimaryGameLayout(const FObjectInitializer& ObjectInitializer);

	/** A dormant root layout is collapsed and responds only to persistent actions registered by the owning player */
	/** 休眠状态下的根布局会被折叠起来，并且仅对由所属玩家注册的持续性操作做出响应 */
	UE_API void SetIsDormant(bool Dormant);
	bool IsDormant() const { return bIsDormant; }

public:
	// 异步推送布局
	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	TSharedPtr<FStreamableHandle> PushWidgetToLayerStackAsync(FGameplayTag LayerName, bool bSuspendInputUntilComplete, TSoftClassPtr<UCommonActivatableWidget> ActivatableWidgetClass)
	{
		return PushWidgetToLayerStackAsync<ActivatableWidgetT>(LayerName, bSuspendInputUntilComplete, ActivatableWidgetClass, [](EAsyncWidgetLayerState, ActivatableWidgetT*) {});
	}

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	TSharedPtr<FStreamableHandle> PushWidgetToLayerStackAsync(FGameplayTag LayerName, bool bSuspendInputUntilComplete, TSoftClassPtr<UCommonActivatableWidget> ActivatableWidgetClass, TFunction<void(EAsyncWidgetLayerState, ActivatableWidgetT*)> StateFunc)
	{
		// 类型判定
		static_assert(TIsDerivedFrom<ActivatableWidgetT, UCommonActivatableWidget>::IsDerived, "Only CommonActivatableWidgets can be used here");

		// 中断的原因
		static FName NAME_PushingWidgetToLayer("PushingWidgetToLayer");
		// 这里可以决定是否要阻断输入
		const FName SuspendInputToken = bSuspendInputUntilComplete ? UCommonUIExtensions::SuspendInputForPlayer(GetOwningPlayer(), NAME_PushingWidgetToLayer) : NAME_None;

		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
		TSharedPtr<FStreamableHandle> StreamingHandle = StreamableManager.RequestAsyncLoad(ActivatableWidgetClass.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this,
			[this, LayerName, ActivatableWidgetClass, StateFunc, SuspendInputToken]()
			{
				// 恢复输入
				UCommonUIExtensions::ResumeInputForPlayer(GetOwningPlayer(), SuspendInputToken);

				// 调用同步接口
				ActivatableWidgetT* Widget = PushWidgetToLayerStack<ActivatableWidgetT>(LayerName, ActivatableWidgetClass.Get(), [StateFunc](ActivatableWidgetT& WidgetToInit) {
					StateFunc(EAsyncWidgetLayerState::Initialize, &WidgetToInit);
				});
				// 外部的回调事件
				StateFunc(EAsyncWidgetLayerState::AfterPush, Widget);
			})
		);

		// Setup a cancel delegate so that we can resume input if this handler is canceled.
		// 设置一个取消委托，以便在该处理程序被取消时能够恢复输入操作。
		StreamingHandle->BindCancelDelegate(FStreamableDelegate::CreateWeakLambda(this,
			[this, StateFunc, SuspendInputToken]()
			{
				UCommonUIExtensions::ResumeInputForPlayer(GetOwningPlayer(), SuspendInputToken);
				StateFunc(EAsyncWidgetLayerState::Canceled, nullptr);
			})
		);

		return StreamingHandle;
	}

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayerStack(FGameplayTag LayerName, UClass* ActivatableWidgetClass)
	{
		return PushWidgetToLayerStack<ActivatableWidgetT>(LayerName, ActivatableWidgetClass, [](ActivatableWidgetT&) {});
	}

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayerStack(FGameplayTag LayerName, UClass* ActivatableWidgetClass, TFunctionRef<void(ActivatableWidgetT&)> InitInstanceFunc)
	{
		static_assert(TIsDerivedFrom<ActivatableWidgetT, UCommonActivatableWidget>::IsDerived, "Only CommonActivatableWidgets can be used here");

		if (UCommonActivatableWidgetContainerBase* Layer = GetLayerWidget(LayerName))
		{
			return Layer->AddWidget<ActivatableWidgetT>(ActivatableWidgetClass, InitInstanceFunc);
		}

		return nullptr;
	}

	// Find the widget if it exists on any of the layers and remove it from the layer.
	// 如果该组件存在于任何一层中，则将其从该层中移除。
	UE_API void FindAndRemoveWidgetFromLayer(UCommonActivatableWidget* ActivatableWidget);

	// Get the layer widget for the given layer tag.
	// 根据给定的图层标签获取该图层的组件。
	UE_API UCommonActivatableWidgetContainerBase* GetLayerWidget(FGameplayTag LayerName);

protected:
	/** Register a layer that widgets can be pushed onto. */
	/** 注册一个可将控件推入其中的层。*/
	UFUNCTION(BlueprintCallable, Category="Layer")
	UE_API void RegisterLayer(UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget);

	// 休眠状态发生改变 触发事件
	UE_API virtual void OnIsDormantChanged();

	// 信号量 控制输入操作的是否暂停
	UE_API void OnWidgetStackTransitioning(UCommonActivatableWidgetContainerBase* Widget, bool bIsTransitioning);
	
private:
	bool bIsDormant = false;

	// Lets us keep track of all suspended input tokens so that multiple async UIs can be loading and we correctly suspend
	// for the duration of all of them.
	// 让我们记录所有被暂停的输入令牌，以便多个异步用户界面能够同时加载，同时我们也能在所有这些操作持续进行期间正确地暂停操作。
	TArray<FName> SuspendInputTokens;

	// The registered layers for the primary layout.
	// 主布局所注册的层。
	UPROPERTY(Transient, meta = (Categories = "UI.Layer"))
	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> Layers;
};

#undef UE_API
