// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "UObject/SoftObjectPath.h"

#include "CommonLoadingScreenSettings.generated.h"

class UObject;

/**
 * Settings for a loading screen system.
 * 加载屏幕系统的设置。
 */
UCLASS(config=Game, defaultconfig, meta=(DisplayName="Common Loading Screen"))
class UCommonLoadingScreenSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	UCommonLoadingScreenSettings();

public:
	
	// The widget to load for the loading screen.
	// 用于加载屏幕的加载组件。
	UPROPERTY(config, EditAnywhere, Category=Display, meta=(MetaClass="/Script/UMG.UserWidget"))
	FSoftClassPath LoadingScreenWidget;

	// The z-order of the loading screen widget in the viewport stack
	// 视图堆栈中加载屏幕组件的层级顺序
	UPROPERTY(config, EditAnywhere, Category=Display)
	int32 LoadingScreenZOrder = 10000;

	// How long to hold the loading screen up after other loading finishes (in seconds) to
	// try to give texture streaming a chance to avoid blurriness
	//
	// Note: This is not normally applied in the editor for iteration time, but can be 
	// enabled via HoldLoadingScreenAdditionalSecsEvenInEditor
	// 其他加载完成之后，要将加载界面保持显示多长时间（以秒为单位），以期让纹理流加载有机会避免出现模糊现象//
	// 注意：通常情况下，此功能不适用于编辑器中的迭代时间，但可以通过“HoldLoadingScreenAdditionalSecsEvenInEditor”选项来启用。
 	UPROPERTY(config, EditAnywhere, Category=Configuration, meta=(ForceUnits=s, ConsoleVariable="CommonLoadingScreen.HoldLoadingScreenAdditionalSecs"))
	float HoldLoadingScreenAdditionalSecs = 2.0f;

	// The interval in seconds beyond which the loading screen is considered permanently hung (if non-zero).
	// 超过此时间间隔（以秒为单位）后，加载界面将被视为永久性卡住（若该值非零）。
 	UPROPERTY(config, EditAnywhere, Category=Configuration, meta=(ForceUnits=s))
	float LoadingScreenHeartbeatHangDuration = 0.0f;

	// The interval in seconds between each log of what is keeping a loading screen up (if non-zero).
	// 每次记录加载屏幕保持状态变化的时间间隔（若不为零）。单位：秒。
 	UPROPERTY(config, EditAnywhere, Category=Configuration, meta=(ForceUnits=s))
	float LogLoadingScreenHeartbeatInterval = 5.0f;

	// When true, the reason the loading screen is shown or hidden will be printed to the log every frame.
	// 若为真，则每次渲染时都会将加载界面显示或隐藏的原因记录到日志中。
	UPROPERTY(Transient, EditAnywhere, Category=Debugging, meta=(ConsoleVariable="CommonLoadingScreen.LogLoadingScreenReasonEveryFrame"))
	bool LogLoadingScreenReasonEveryFrame = 0;

	// Force the loading screen to be displayed (useful for debugging)
	// 强制显示加载界面（用于调试之用）
	UPROPERTY(Transient, EditAnywhere, Category=Debugging, meta=(ConsoleVariable="CommonLoadingScreen.AlwaysShow"))
	bool ForceLoadingScreenVisible = false;

	// Should we apply the additional HoldLoadingScreenAdditionalSecs delay even in the editor
	// (useful when iterating on loading screens)
	// 我们是否应该在编辑器中也使用额外的“保持加载界面显示时间（额外秒数）”这一设置呢？
	// （在对加载界面进行改进时，此设置很有用）
	UPROPERTY(Transient, EditAnywhere, Category=Debugging)
	bool HoldLoadingScreenAdditionalSecsEvenInEditor = false;

	// Should we apply the additional HoldLoadingScreenAdditionalSecs delay even in the editor
	// (useful when iterating on loading screens)
	// 我们是否应该在编辑器中也使用额外的“保持加载界面显示时间（额外秒数）”这一设置呢？
	// （在对加载界面进行改进时，此设置很有用）
	UPROPERTY(config, EditAnywhere, Category=Configuration)
	bool ForceTickLoadingScreenEvenInEditor = true;
};

