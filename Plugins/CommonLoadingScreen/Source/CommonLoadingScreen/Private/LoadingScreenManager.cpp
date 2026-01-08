// Copyright Epic Games, Inc. All Rights Reserved.

#include "LoadingScreenManager.h"

#include "HAL/ThreadHeartBeat.h"

#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Engine.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/WorldSettings.h"
#include "Misc/CommandLine.h"
#include "Misc/ConfigCacheIni.h"

#include "LoadingProcessInterface.h"

#include "Framework/Application/IInputProcessor.h"
#include "Framework/Application/SlateApplication.h"

#include "PreLoadScreen.h"
#include "PreLoadScreenManager.h"

#include "ShaderPipelineCache.h"
#include "CommonLoadingScreenSettings.h"

//@TODO: Used as the placeholder widget in error cases, should probably create a wrapper that at least centers it/etc...
#include "Widgets/Images/SThrobber.h"
#include "Blueprint/UserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LoadingScreenManager)

DECLARE_LOG_CATEGORY_EXTERN(LogLoadingScreen, Log, All);
DEFINE_LOG_CATEGORY(LogLoadingScreen);

//@TODO: Why can GetLocalPlayers() have nullptr entries?  Can it really?
//@TODO: Test with PIE mode set to simulate and decide how much (if any) loading screen action should occur
//@TODO: Allow other things implementing ILoadingProcessInterface besides GameState/PlayerController (and owned components) to register as interested parties
//@TODO: ChangeMusicSettings (either here or using the LoadingScreenVisibilityChanged delegate)
//@TODO: Studio analytics (FireEvent_PIEFinishedLoading / tracking PIE startup time for regressions, either here or using the LoadingScreenVisibilityChanged delegate)


//@待办事项：为何 GetLocalPlayers() 可能会出现空值条目？这真的有可能吗？
//@待办事项：在 PIE 模式设置为模拟的情况下进行测试，并决定加载屏幕操作应进行到何种程度（如果有的话）
//@待办事项：允许除了 GameState/PlayerController（以及所属组件）之外的其他实现 ILoadingProcessInterface 的对象也注册为感兴趣方
//@待办事项：更改音乐设置（可以在这里进行，也可以通过 LoadingScreenVisibilityChanged 事件委托来实现）
//@待办事项：工作室分析（使用 FireEvent_PIEFinishedLoading 或在 LoadingScreenVisibilityChanged 事件委托中跟踪 PIE 启动时间以进行回归测试）


// Profiling category for loading screens
// 负载界面的分类分析
CSV_DEFINE_CATEGORY(LoadingScreen, true);

//////////////////////////////////////////////////////////////////////

bool ILoadingProcessInterface::ShouldShowLoadingScreen(UObject* TestObject, FString& OutReason)
{
	if (TestObject != nullptr)
	{
		if (ILoadingProcessInterface* LoadObserver = Cast<ILoadingProcessInterface>(TestObject))
		{
			FString ObserverReason;
			if (LoadObserver->ShouldShowLoadingScreen(/*out*/ ObserverReason))
			{
				if (ensureMsgf(!ObserverReason.IsEmpty(), TEXT("%s failed to set a reason why it wants to show the loading screen"), *GetPathNameSafe(TestObject)))
				{
					OutReason = ObserverReason;
				}
				return true;
			}
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////

namespace LoadingScreenCVars
{
	// CVars
	// 在其他加载过程完成后，将加载界面保持显示状态的时长（以秒为单位）应设置为多少，以便让纹理流处理机制有时间避免出现模糊现象？
	static float HoldLoadingScreenAdditionalSecs = 2.0f;
	static FAutoConsoleVariableRef CVarHoldLoadingScreenUpAtLeastThisLongInSecs(
		TEXT("CommonLoadingScreen.HoldLoadingScreenAdditionalSecs"),
		HoldLoadingScreenAdditionalSecs,
		TEXT("How long to hold the loading screen up after other loading finishes (in seconds) to try to give texture streaming a chance to avoid blurriness"),
		ECVF_Default | ECVF_Preview);

	// 如果情况属实，那么显示或隐藏加载界面的原因将会在每一帧中被记录到日志中。
	static bool LogLoadingScreenReasonEveryFrame = false;
	static FAutoConsoleVariableRef CVarLogLoadingScreenReasonEveryFrame(
		TEXT("CommonLoadingScreen.LogLoadingScreenReasonEveryFrame"),
		LogLoadingScreenReasonEveryFrame,
		TEXT("When true, the reason the loading screen is shown or hidden will be printed to the log every frame."),
		ECVF_Default);

	// 强制显示加载界面。
	static bool ForceLoadingScreenVisible = false;
	static FAutoConsoleVariableRef CVarForceLoadingScreenVisible(
		TEXT("CommonLoadingScreen.AlwaysShow"),
		ForceLoadingScreenVisible,
		TEXT("Force the loading screen to show."),
		ECVF_Default);
}

//////////////////////////////////////////////////////////////////////
// FLoadingScreenInputPreProcessor

// Input processor to throw in when loading screen is shown
// This will capture any inputs, so active menus under the loading screen will not interact
// 在加载界面显示时使用的输入处理器
// 此处理器将捕获所有输入操作，因此加载界面下的活动菜单将不会与之交互
class FLoadingScreenInputPreProcessor : public IInputProcessor
{
public:
	FLoadingScreenInputPreProcessor() { }
	virtual ~FLoadingScreenInputPreProcessor() { }

	bool CanEatInput() const
	{
		return !GIsEditor;
	}

	//~IInputProcess interface
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override { }

	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override { return CanEatInput(); }
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override { return CanEatInput(); }
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override { return CanEatInput(); }
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanEatInput(); }
	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanEatInput(); }
	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanEatInput(); }
	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanEatInput(); }
	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) override { return CanEatInput(); }
	virtual bool HandleMotionDetectedEvent(FSlateApplication& SlateApp, const FMotionEvent& MotionEvent) override { return CanEatInput(); }
	//~End of IInputProcess interface
};

//////////////////////////////////////////////////////////////////////
// ULoadingScreenManager

void ULoadingScreenManager::Initialize(FSubsystemCollectionBase& Collection)
{
	/** 在加载地图操作之初发送 */
	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &ThisClass::HandlePreLoadMap);

	/** 在加载地图操作完成后发送 */
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandlePostLoadMap);

	const UGameInstance* LocalGameInstance = GetGameInstance();
	check(LocalGameInstance);
}

void ULoadingScreenManager::Deinitialize()
{
	StopBlockingInput();

	RemoveWidgetFromViewport();

	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	// We are done, so do not attempt to tick us again
	// 我们已经完成了，所以不要再试图催促我们了
	SetTickableTickType(ETickableTickType::Never);
}

bool ULoadingScreenManager::ShouldCreateSubsystem(UObject* Outer) const
{
	// Only clients have loading screens
	// 仅客户端会有加载界面
	const UGameInstance* GameInstance = CastChecked<UGameInstance>(Outer);
	const bool bIsServerWorld = GameInstance->IsDedicatedServerInstance();	
	return !bIsServerWorld;
}

void ULoadingScreenManager::Tick(float DeltaTime)
{
	UpdateLoadingScreen();

	TimeUntilNextLogHeartbeatSeconds = FMath::Max(TimeUntilNextLogHeartbeatSeconds - DeltaTime, 0.0);
}

ETickableTickType ULoadingScreenManager::GetTickableTickType() const
{
	if (IsTemplate())
	{
		return ETickableTickType::Never;
	}
	return ETickableTickType::Conditional;
}

bool ULoadingScreenManager::IsTickable() const
{
	// Don't tick if we don't have a game viewport client, this catches cases that ShouldCreateSubsystem does not
	// 若我们没有游戏视口客户端，则不要勾选此选项，因为这能涵盖“ShouldCreateSubsystem”函数未处理的情况。
	UGameInstance* GameInstance = GetGameInstance();
	return (GameInstance && GameInstance->GetGameViewportClient());
}

TStatId ULoadingScreenManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(ULoadingScreenManager, STATGROUP_Tickables);
}

UWorld* ULoadingScreenManager::GetTickableGameObjectWorld() const
{
	return GetGameInstance()->GetWorld();
}

void ULoadingScreenManager::RegisterLoadingProcessor(TScriptInterface<ILoadingProcessInterface> Interface)
{
	ExternalLoadingProcessors.Add(Interface.GetObject());
}

void ULoadingScreenManager::UnregisterLoadingProcessor(TScriptInterface<ILoadingProcessInterface> Interface)
{
	ExternalLoadingProcessors.Remove(Interface.GetObject());
}

void ULoadingScreenManager::HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName)
{
	if (WorldContext.OwningGameInstance == GetGameInstance())
	{
		bCurrentlyInLoadMap = true;

		// Update the loading screen immediately if the engine is initialized
		// 若引擎已初始化，则立即更新加载界面
		if (GEngine->IsInitialized())
		{
			UpdateLoadingScreen();
		}
	}
}

void ULoadingScreenManager::HandlePostLoadMap(UWorld* World)
{
	if ((World != nullptr) && (World->GetGameInstance() == GetGameInstance()))
	{
		bCurrentlyInLoadMap = false;
	}
}

void ULoadingScreenManager::UpdateLoadingScreen()
{
	bool bLogLoadingScreenStatus = LoadingScreenCVars::LogLoadingScreenReasonEveryFrame;

	if (ShouldShowLoadingScreen())
	{
		const UCommonLoadingScreenSettings* Settings = GetDefault<UCommonLoadingScreenSettings>();
		
		// If we don't make it to the specified checkpoint in the given time will trigger the hang detector so we can better determine where progress stalled.
		// 如果我们未能在规定时间内到达指定的检查点，就会触发挂起检测机制，这样我们就能更清楚地了解进度停滞的具体位置。
 		FThreadHeartBeat::Get().MonitorCheckpointStart(GetFName(), Settings->LoadingScreenHeartbeatHangDuration);

		ShowLoadingScreen();

 		if ((Settings->LogLoadingScreenHeartbeatInterval > 0.0f) && (TimeUntilNextLogHeartbeatSeconds <= 0.0))
 		{
			bLogLoadingScreenStatus = true;
 			TimeUntilNextLogHeartbeatSeconds = Settings->LogLoadingScreenHeartbeatInterval;
 		}
	}
	else
	{
		HideLoadingScreen();
		/* 当检查点结束时，由线程调用 */
 		FThreadHeartBeat::Get().MonitorCheckpointEnd(GetFName());
	}

	if (bLogLoadingScreenStatus)
	{
		UE_LOG(LogLoadingScreen, Log, TEXT("Loading screen showing: %d. Reason: %s"), bCurrentlyShowingLoadingScreen ? 1 : 0, *DebugReasonForShowingOrHidingLoadingScreen);
	}
}

bool ULoadingScreenManager::CheckForAnyNeedToShowLoadingScreen()
{
	// Start out with 'unknown' reason in case someone forgets to put a reason when changing this in the future.
	// 一开始将原因设为“未知”，以防日后有人在修改此项内容时忘记填写原因。
	DebugReasonForShowingOrHidingLoadingScreen = TEXT("Reason for Showing/Hiding LoadingScreen is unknown!");

	const UGameInstance* LocalGameInstance = GetGameInstance();

	// 是否强制显示
	if (LoadingScreenCVars::ForceLoadingScreenVisible)
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("CommonLoadingScreen.AlwaysShow is true"));
		return true;
	}

	const FWorldContext* Context = LocalGameInstance->GetWorldContext();
	if (Context == nullptr)
	{
		// We don't have a world context right now... better show a loading screen
		// 目前我们还没有全球性的背景信息……最好显示一个加载界面
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("The game instance has a null WorldContext"));
		return true;
	}

	UWorld* World = Context->World();
	// 无世界
	if (World == nullptr)
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("We have no world (FWorldContext's World() is null)"));
		return true;
	}

	AGameStateBase* GameState = World->GetGameState<AGameStateBase>();
	// 无GameState
	if (GameState == nullptr)
	{
		// The game state has not yet replicated.
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("GameState hasn't yet replicated (it's null)"));
		return true;
	}

	// 加载地图中
	if (bCurrentlyInLoadMap)
	{
		// Show a loading screen if we are in LoadMap
		// 如果处于“加载地图”状态，则显示加载界面
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("bCurrentlyInLoadMap is true"));
		return true;
	}

	/** 用于处理待连接客户端的跳转 URL */
	if (!Context->TravelURL.IsEmpty())
	{
		// Show a loading screen when pending travel
		// 当有待处理的行程时显示加载界面
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("We have pending travel (the TravelURL is not empty)"));
		return true;
	}

	if (Context->PendingNetGame != nullptr)
	{
		// Connecting to another server
		// 连接到另一台服务器
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("We are connecting to another server (PendingNetGame != nullptr)"));
		return true;
	}

	// 世界还没开始
	if (!World->HasBegunPlay())
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("World hasn't begun play"));
		return true;
	}

	if (World->IsInSeamlessTravel())
	{
		// Show a loading screen during seamless travel
		// 在无缝切换过程中显示加载界面
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("We are in seamless travel"));
		return true;
	}

	// Ask the game state if it needs a loading screen
	// 调用游戏状态对象，询问其是否需要加载界面
	if (ILoadingProcessInterface::ShouldShowLoadingScreen(GameState, /*out*/ DebugReasonForShowingOrHidingLoadingScreen))
	{
		return true;
	}

	// Ask any game state components if they need a loading screen
	// 要求任何游戏状态组件告知自身是否需要加载界面
	for (UActorComponent* TestComponent : GameState->GetComponents())
	{
		if (ILoadingProcessInterface::ShouldShowLoadingScreen(TestComponent, /*out*/ DebugReasonForShowingOrHidingLoadingScreen))
		{
			return true;
		}
	}

	// Ask any of the external loading processors that may have been registered.  These might be actors or components
	// that were registered by game code to tell us to keep the loading screen up while perhaps something finishes
	// streaming in.
	// 联系任何可能已注册的外部加载处理器。这些处理器可能是由游戏代码注册的演员或组件，它们会告知我们应保持加载界面处于显示状态，以便在某些内容完成加载传输时能够显示出来。
	for (const TWeakInterfacePtr<ILoadingProcessInterface>& Processor : ExternalLoadingProcessors)
	{
		if (ILoadingProcessInterface::ShouldShowLoadingScreen(Processor.GetObject(), /*out*/ DebugReasonForShowingOrHidingLoadingScreen))
		{
			return true;
		}
	}

	// Check each local player
	// 检查每个本地玩家
	bool bFoundAnyLocalPC = false;
	bool bMissingAnyLocalPC = false;

	for (ULocalPlayer* LP : LocalGameInstance->GetLocalPlayers())
	{
		if (LP != nullptr)
		{
			if (APlayerController* PC = LP->PlayerController)
			{
				bFoundAnyLocalPC = true;

				// Ask the PC itself if it needs a loading screen
				// 要求电脑自身确认是否需要加载画面
				if (ILoadingProcessInterface::ShouldShowLoadingScreen(PC, /*out*/ DebugReasonForShowingOrHidingLoadingScreen))
				{
					return true;
				}

				// Ask any PC components if they need a loading screen
				// 询问任何电脑组件是否需要加载画面
				for (UActorComponent* TestComponent : PC->GetComponents())
				{
					if (ILoadingProcessInterface::ShouldShowLoadingScreen(TestComponent, /*out*/ DebugReasonForShowingOrHidingLoadingScreen))
					{
						return true;
					}
				}
			}
			else
			{
				bMissingAnyLocalPC = true;
			}
		}
	}

	UGameViewportClient* GameViewportClient = LocalGameInstance->GetGameViewportClient();
	const bool bIsInSplitscreen = GameViewportClient->GetCurrentSplitscreenConfiguration() != ESplitScreenType::None;

	// In splitscreen we need all player controllers to be present
	// 在分屏模式下，我们需要确保所有玩家控制器都处于开启状态
	if (bIsInSplitscreen && bMissingAnyLocalPC)
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("At least one missing local player controller in splitscreen"));
		return true;
	}

	// And in non-splitscreen we need at least one player controller to be present
	// 在非分屏模式下，我们至少需要有一个玩家控制器在场。
	if (!bIsInSplitscreen && !bFoundAnyLocalPC)
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("Need at least one local player controller"));
		return true;
	}

	// Victory! The loading screen can go away now
	// 胜利！加载画面现在可以消失了
	DebugReasonForShowingOrHidingLoadingScreen = TEXT("(nothing wants to show it anymore)");
	return false;
}

bool ULoadingScreenManager::ShouldShowLoadingScreen()
{
	const UCommonLoadingScreenSettings* Settings = GetDefault<UCommonLoadingScreenSettings>();

	// Check debugging commands that force the state one way or another
	// 检查那些能够强制改变状态的调试命令
#if !UE_BUILD_SHIPPING
	// 命令行无加载界面
	static bool bCmdLineNoLoadingScreen = FParse::Param(FCommandLine::Get(), TEXT("NoLoadingScreen"));
	if (bCmdLineNoLoadingScreen)
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("CommandLine has 'NoLoadingScreen'"));
		return false;
	}
#endif

	// Can't show a loading screen if there's no game viewport
	// 若没有游戏视图区域，则无法显示加载屏幕
	UGameInstance* LocalGameInstance = GetGameInstance();
	if (LocalGameInstance->GetGameViewportClient() == nullptr)
	{
		return false;
	}

	// Check for a need to show the loading screen
	// 检查是否需要显示加载界面
	const bool bNeedToShowLoadingScreen = CheckForAnyNeedToShowLoadingScreen();

	// Keep the loading screen up a bit longer if desired
	// 如果需要的话，可以让加载界面多显示一段时间
	bool bWantToForceShowLoadingScreen = false;
	if (bNeedToShowLoadingScreen)
	{
		// Still need to show it
		// 仍需展示它
		TimeLoadingScreenLastDismissed = -1.0;
	}
	else
	{
		// Don't *need* to show the screen anymore, but might still want to for a bit
		// 无需再显示该屏幕了，但可能仍需要展示一小段时间。
		const double CurrentTime = FPlatformTime::Seconds();
		const bool bCanHoldLoadingScreen = (!GIsEditor || Settings->HoldLoadingScreenAdditionalSecsEvenInEditor);
		const double HoldLoadingScreenAdditionalSecs = bCanHoldLoadingScreen ? LoadingScreenCVars::HoldLoadingScreenAdditionalSecs : 0.0;

		if (TimeLoadingScreenLastDismissed < 0.0)
		{
			TimeLoadingScreenLastDismissed = CurrentTime;
		}
		const double TimeSinceScreenDismissed = CurrentTime - TimeLoadingScreenLastDismissed;

		// hold for an extra X seconds, to cover up streaming
		// 延长等待 X 秒，以弥补数据传输的延迟
		if ((HoldLoadingScreenAdditionalSecs > 0.0) && (TimeSinceScreenDismissed < HoldLoadingScreenAdditionalSecs))
		{
			// Make sure we're rendering the world at this point, so that textures will actually stream in
			//@TODO: If bNeedToShowLoadingScreen bounces back true during this window, we won't turn this off again...
			
			// 确保此时我们正在渲染整个世界，这样纹理才能真正加载进来
			//@待办事项：如果在这一窗口期间 bNeedToShowLoadingScreen 的值反向变为 true，我们就不会再次将其关闭……
			
			UGameViewportClient* GameViewportClient = GetGameInstance()->GetGameViewportClient();
			/** 是否设置为禁用世界渲染 */
			// 开始渲染3D画面
			GameViewportClient->bDisableWorldRendering = false;

			DebugReasonForShowingOrHidingLoadingScreen = FString::Printf(TEXT("Keeping loading screen up for an additional %.2f seconds to allow texture streaming"), HoldLoadingScreenAdditionalSecs);
			bWantToForceShowLoadingScreen = true;
		}
	}

	return bNeedToShowLoadingScreen || bWantToForceShowLoadingScreen;
}

bool ULoadingScreenManager::IsShowingInitialLoadingScreen() const
{
	FPreLoadScreenManager* PreLoadScreenManager = FPreLoadScreenManager::Get();
	return (PreLoadScreenManager != nullptr) && PreLoadScreenManager->HasValidActivePreLoadScreen();
}

void ULoadingScreenManager::ShowLoadingScreen()
{
	// 已经在显示了 就不需要处理
	if (bCurrentlyShowingLoadingScreen)
	{
		return;
	}

	// Unable to show loading screen if the engine is still loading with its loading screen.
	// 如果引擎仍在加载且带有加载界面，就无法显示加载屏幕。
	if (FPreLoadScreenManager::Get() && FPreLoadScreenManager::Get()->HasActivePreLoadScreenType(EPreLoadScreenTypes::EngineLoadingScreen))
	{
		return;
	}

	TimeLoadingScreenShown = FPlatformTime::Seconds();

	bCurrentlyShowingLoadingScreen = true;

	CSV_EVENT(LoadingScreen, TEXT("Show"));

	const UCommonLoadingScreenSettings* Settings = GetDefault<UCommonLoadingScreenSettings>();

	// 如果有初始化的早期初始化界面就不管
	if (IsShowingInitialLoadingScreen())
	{
		UE_LOG(LogLoadingScreen, Log, TEXT("Showing loading screen when 'IsShowingInitialLoadingScreen()' is true."));
		UE_LOG(LogLoadingScreen, Log, TEXT("%s"), *DebugReasonForShowingOrHidingLoadingScreen);
	}
	else
	{
		UE_LOG(LogLoadingScreen, Log, TEXT("Showing loading screen when 'IsShowingInitialLoadingScreen()' is false."));
		UE_LOG(LogLoadingScreen, Log, TEXT("%s"), *DebugReasonForShowingOrHidingLoadingScreen);

		UGameInstance* LocalGameInstance = GetGameInstance();

		// Eat input while the loading screen is displayed
		// 在加载界面显示期间食用输入的内容
		StartBlockingInput();

		LoadingScreenVisibilityChanged.Broadcast(/*bIsVisible=*/ true);

		// Create the loading screen widget
		// 创建加载界面组件
		TSubclassOf<UUserWidget> LoadingScreenWidgetClass = Settings->LoadingScreenWidget.TryLoadClass<UUserWidget>();
		if (UUserWidget* UserWidget = UUserWidget::CreateWidgetInstance(*LocalGameInstance, LoadingScreenWidgetClass, NAME_None))
		{
			LoadingScreenWidget = UserWidget->TakeWidget();
		}
		else
		{
			UE_LOG(LogLoadingScreen, Error, TEXT("Failed to load the loading screen widget %s, falling back to placeholder."), *Settings->LoadingScreenWidget.ToString());
			LoadingScreenWidget = SNew(SThrobber);
		}

		// Add to the viewport at a high ZOrder to make sure it is on top of most things
		// 将其添加至视口中，并设置较高的 Z 顺序，以确保其位于大多数元素之上
		UGameViewportClient* GameViewportClient = LocalGameInstance->GetGameViewportClient();
		GameViewportClient->AddViewportWidgetContent(LoadingScreenWidget.ToSharedRef(), Settings->LoadingScreenZOrder);

		ChangePerformanceSettings(/*bEnableLoadingScreen=*/ true);

		if (!GIsEditor || Settings->ForceTickLoadingScreenEvenInEditor)
		{
			// Tick Slate to make sure the loading screen is displayed immediately
			// 点击“Slate”按钮以确保加载界面能立即显示出来
			FSlateApplication::Get().Tick();
		}
	}
}

void ULoadingScreenManager::HideLoadingScreen()
{
	if (!bCurrentlyShowingLoadingScreen)
	{
		return;
	}

	// 恢复输入
	StopBlockingInput();

	if (IsShowingInitialLoadingScreen())
	{
		UE_LOG(LogLoadingScreen, Log, TEXT("Hiding loading screen when 'IsShowingInitialLoadingScreen()' is true."));
		UE_LOG(LogLoadingScreen, Log, TEXT("%s"), *DebugReasonForShowingOrHidingLoadingScreen);
	}
	else
	{
		UE_LOG(LogLoadingScreen, Log, TEXT("Hiding loading screen when 'IsShowingInitialLoadingScreen()' is false."));
		UE_LOG(LogLoadingScreen, Log, TEXT("%s"), *DebugReasonForShowingOrHidingLoadingScreen);

		UE_LOG(LogLoadingScreen, Log, TEXT("Garbage Collecting before dropping load screen"));
		GEngine->ForceGarbageCollection(true);

		RemoveWidgetFromViewport();
	
		ChangePerformanceSettings(/*bEnableLoadingScreen=*/ false);

		// Let observers know that the loading screen is done
		// 通知观察者加载界面已完成
		LoadingScreenVisibilityChanged.Broadcast(/*bIsVisible=*/ false);
	}

	CSV_EVENT(LoadingScreen, TEXT("Hide"));

	const double LoadingScreenDuration = FPlatformTime::Seconds() - TimeLoadingScreenShown;
	UE_LOG(LogLoadingScreen, Log, TEXT("LoadingScreen was visible for %.2fs"), LoadingScreenDuration);

	bCurrentlyShowingLoadingScreen = false;
}

void ULoadingScreenManager::RemoveWidgetFromViewport()
{
	UGameInstance* LocalGameInstance = GetGameInstance();
	if (LoadingScreenWidget.IsValid())
	{
		if (UGameViewportClient* GameViewportClient = LocalGameInstance->GetGameViewportClient())
		{
			GameViewportClient->RemoveViewportWidgetContent(LoadingScreenWidget.ToSharedRef());
		}
		LoadingScreenWidget.Reset();
	}
}

void ULoadingScreenManager::StartBlockingInput()
{
	if (!InputPreProcessor.IsValid())
	{
		InputPreProcessor = MakeShareable<FLoadingScreenInputPreProcessor>(new FLoadingScreenInputPreProcessor());
		FSlateApplication::Get().RegisterInputPreProcessor(InputPreProcessor, 0);
	}
}

void ULoadingScreenManager::StopBlockingInput()
{
	if (InputPreProcessor.IsValid())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(InputPreProcessor);
		InputPreProcessor.Reset();
	}
}

void ULoadingScreenManager::ChangePerformanceSettings(bool bEnabingLoadingScreen)
{
	UGameInstance* LocalGameInstance = GetGameInstance();
	UGameViewportClient* GameViewportClient = LocalGameInstance->GetGameViewportClient();
	/** 设置预编译批处理模式。*/
	FShaderPipelineCache::SetBatchMode(bEnabingLoadingScreen ? FShaderPipelineCache::BatchMode::Fast : FShaderPipelineCache::BatchMode::Background);

	// Don't bother drawing the 3D world while we're loading
	// 在加载过程中请勿绘制三维世界画面
	GameViewportClient->bDisableWorldRendering = bEnabingLoadingScreen;

	// Make sure to prioritize streaming in levels if the loading screen is up
	// 若加载界面处于显示状态，则务必优先进行流媒体播放设置
	if (UWorld* ViewportWorld = GameViewportClient->GetWorld())
	{
		if (AWorldSettings* WorldSettings = ViewportWorld->GetWorldSettings(false, false))
		{
			/** 未进行复制的 bHighPriorityLoading 的副本，专用于客户端仅有的加载操作 */
			/** 当此标志被设置时，将为后台加载（复制操作）分配更多时间 */
			WorldSettings->bHighPriorityLoadingLocal = bEnabingLoadingScreen;
		}
	}

	if (bEnabingLoadingScreen)
	{
		// Set a new hang detector timeout multiplier when the loading screen is visible.
		// 当加载界面可见时，设置一个新的挂起检测超时倍数。
		double HangDurationMultiplier;
		if (!GConfig || !GConfig->GetDouble(TEXT("Core.System"), TEXT("LoadingScreenHangDurationMultiplier"), /*out*/ HangDurationMultiplier, GEngineIni))
		{
			HangDurationMultiplier = 1.0;
		}
		/**
		* 将悬挂时间的倍数设置为大于等于 1.0 的值。
		* 可用于在加载界面等场景中延长悬挂时间。*/
		FThreadHeartBeat::Get().SetDurationMultiplier(HangDurationMultiplier);

		// Do not report hitches while the loading screen is up
		// 在加载界面显示期间，请勿报告任何故障情况
		FGameThreadHitchHeartBeat::Get().SuspendHeartBeat();
	}
	else
	{
		// Restore the hang detector timeout when we hide the loading screen
		// 当我们隐藏加载屏幕时，恢复挂起检测器的超时设置
		FThreadHeartBeat::Get().SetDurationMultiplier(1.0);

		// Resume reporting hitches now that the loading screen is down
		// 在加载界面关闭后恢复报告功能
		FGameThreadHitchHeartBeat::Get().ResumeHeartBeat();
	}
}

