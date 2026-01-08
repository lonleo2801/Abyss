// Copyright Epic Games, Inc. All Rights Reserved.
// Finished.
#pragma once

#include "CommonUserSubsystem.h"
#include "Engine/CancellableAsyncAction.h"

#include "AsyncAction_CommonUserInitialize.generated.h"

#define UE_API COMMONUSER_API

enum class ECommonUserOnlineContext : uint8;
enum class ECommonUserPrivilege : uint8;
struct FInputDeviceId;

class FText;
class UObject;
struct FFrame;

/**
 * Async action to handle different functions for initializing users
 * 异步操作，用于处理针对用户初始化的不同功能
 */
UCLASS(MinimalAPI)
class UAsyncAction_CommonUserInitialize : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:
	/**
	 * Initializes a local player with the common user system, which includes doing platform-specific login and privilege checks.
	 * When the process has succeeded or failed, it will broadcast the OnInitializationComplete delegate.
	 *
	 * @param LocalPlayerIndex	Desired index of ULocalPlayer in Game Instance, 0 will be primary player and 1+ for local multiplayer
	 * @param PrimaryInputDevice Primary input device for the user, if invalid will use the system default
	 * @param bCanUseGuestLogin	If true, this player can be a guest without a real system net id
	 */
	/**
	 * 使用通用用户系统初始化本地玩家，这包括进行特定平台的登录和权限检查。
	 * 当该过程成功或失败时，它会广播“初始化完成”委托。
	 * @参数 LocalPlayerIndex：游戏实例中 ULocalPlayer 的期望索引值，0 表示为主玩家，1 及以上表示本地多人游戏模式
	 * @参数 PrimaryInputDevice：用户的主要输入设备，无效时将使用系统默认设置
	 * @参数 bCanUseGuestLogin：如果为真，则此玩家可以作为访客存在，无需真实的系统网络标识符
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category = CommonUser, meta = (BlueprintInternalUseOnly = "true"))
	static UE_API UAsyncAction_CommonUserInitialize* InitializeForLocalPlay(UCommonUserSubsystem* Target, int32 LocalPlayerIndex, FInputDeviceId PrimaryInputDevice, bool bCanUseGuestLogin);

	/**
	 * Attempts to log an existing user into the platform-specific online backend to enable full online play
	 * When the process has succeeded or failed, it will broadcast the OnInitializationComplete delegate.
	 *
	 * @param LocalPlayerIndex	Index of existing LocalPlayer in Game Instance
	 */
	/**
	 * 尝试让现有用户登录到平台特定的在线后端，以实现完全的在线游戏功能
	 * 当该过程成功或失败时，会广播“OnInitializationComplete”委托。
	 * @参数 LocalPlayerIndex：游戏实例中现有本地玩家的索引号
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category = CommonUser, meta = (BlueprintInternalUseOnly = "true"))
	static UE_API UAsyncAction_CommonUserInitialize* LoginForOnlinePlay(UCommonUserSubsystem* Target, int32 LocalPlayerIndex);

	/** Call when initialization succeeds or fails */
	/** 当初始化成功或失败时调用 */
	UPROPERTY(BlueprintAssignable)
	FCommonUserOnInitializeCompleteMulticast OnInitializationComplete;

	/** Fail and send callbacks if needed */
	/** 若有必要，则在失败时调用回调函数 */
	UE_API void HandleFailure();

	/** Wrapper delegate, will pass on to OnInitializationComplete if appropriate */
	/** 封装委托，若情况适宜则会将其传递给“初始化完成”事件 */
	UFUNCTION()
	UE_API virtual void HandleInitializationComplete(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext);

protected:
	/** Actually start the initialization */
	/** 实际开始初始化操作 */
	UE_API virtual void Activate() override;

	TWeakObjectPtr<UCommonUserSubsystem> Subsystem;
	FCommonUserInitializeParams Params;
};

#undef UE_API
