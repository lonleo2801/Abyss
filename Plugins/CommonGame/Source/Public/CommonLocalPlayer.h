// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/LocalPlayer.h"

#include "CommonLocalPlayer.generated.h"

#define UE_API COMMONGAME_API

class APawn;
class APlayerController;
class APlayerState;
class FViewport;
class UObject;
class UPrimaryGameLayout;
struct FSceneViewProjectionData;

UCLASS(MinimalAPI, config=Engine, transient)
class UCommonLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

public:
	UE_API UCommonLocalPlayer();

	/** Called when the local player is assigned a player controller */
	/** 当本地玩家被分配到玩家控制器时会调用此函数 */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerControllerSetDelegate, UCommonLocalPlayer* LocalPlayer, APlayerController* PlayerController);
	FPlayerControllerSetDelegate OnPlayerControllerSet;

	/** Called when the local player is assigned a player state */
	/** 当本地玩家被分配到特定玩家状态时会调用此函数 */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerStateSetDelegate, UCommonLocalPlayer* LocalPlayer, APlayerState* PlayerState);
	FPlayerStateSetDelegate OnPlayerStateSet;

	/** Called when the local player is assigned a player pawn */
	/** 当本地玩家获得一个玩家棋子时会调用此函数 */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerPawnSetDelegate, UCommonLocalPlayer* LocalPlayer, APawn* Pawn);
	FPlayerPawnSetDelegate OnPlayerPawnSet;

	UE_API FDelegateHandle CallAndRegister_OnPlayerControllerSet(FPlayerControllerSetDelegate::FDelegate Delegate);
	UE_API FDelegateHandle CallAndRegister_OnPlayerStateSet(FPlayerStateSetDelegate::FDelegate Delegate);
	UE_API FDelegateHandle CallAndRegister_OnPlayerPawnSet(FPlayerPawnSetDelegate::FDelegate Delegate);

public:
	/**
	* 用于获取投影所需的各种数据的辅助函数*
	* @参数	视口				视图客户端的视口
	* @参数	投影数据			用于填充投影数据的结构体
	* @参数	立体视图索引		使用立体显示时视图的索引
	* @返回 值为假的情况包括：没有视口，或者该 Actor 为 null 。
	* 
	*/
	UE_API virtual bool GetProjectionData(FViewport* Viewport, FSceneViewProjectionData& ProjectionData, int32 StereoViewIndex) const override;

	bool IsPlayerViewEnabled() const { return bIsPlayerViewEnabled; }
	void SetIsPlayerViewEnabled(bool bInIsPlayerViewEnabled) { bIsPlayerViewEnabled = bInIsPlayerViewEnabled; }

	//拿到主要用户界面布局.
	UE_API UPrimaryGameLayout* GetRootUILayout() const;

private:
	bool bIsPlayerViewEnabled = true;
};

#undef UE_API
