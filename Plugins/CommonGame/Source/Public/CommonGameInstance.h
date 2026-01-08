// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/GameInstance.h"

#include "CommonGameInstance.generated.h"

#define UE_API COMMONGAME_API

enum class ECommonUserAvailability : uint8;
enum class ECommonUserPrivilege : uint8;

class FText;
class UCommonUserInfo;
class UCommonSession_SearchResult;
struct FOnlineResultInformation;
class ULocalPlayer;
class USocialManager;
class UObject;
struct FFrame;
struct FGameplayTag;

UCLASS(MinimalAPI, Abstract, Config = Game)
class UCommonGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UE_API UCommonGameInstance(const FObjectInitializer& ObjectInitializer);
	
	/** Handles errors/warnings from CommonUser, can be overridden per game */
	/** 处理来自“CommonUser”模块的错误/警告信息，可根据游戏需求进行重写 */
	UFUNCTION()
	UE_API virtual void HandleSystemMessage(FGameplayTag MessageType, FText Title, FText Message);

	UFUNCTION()
	UE_API virtual void HandlePrivilegeChanged(const UCommonUserInfo* UserInfo, ECommonUserPrivilege Privilege, ECommonUserAvailability OldAvailability, ECommonUserAvailability NewAvailability);

	UFUNCTION()
	UE_API virtual void HandlerUserInitialized(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext);

	/** Call to reset user and session state, usually because a player has been disconnected */
	/** 调用此函数以重置用户和会话状态，通常是因为玩家已断开连接 */
	UE_API virtual void ResetUserAndSessionState();

	/**
	 * Requested Session Flow
	 *   Something requests the user to join a specific session (for example, a platform overlay via OnUserRequestedSession).
	 *   This request is handled in SetRequestedSession.
	 *   Check if we can join the requested session immediately (CanJoinRequestedSession).  If we can, join the requested session (JoinRequestedSession)
	 *   If not, cache the requested session and instruct the game to get into a state where the session can be joined (ResetGameAndJoinRequestedSession)
	 *
	 * 请求的会话流程
	 *   有某种情况要求用户加入特定的会话（例如，通过“OnUserRequestedSession”调用平台覆盖界面）。
	 *   此请求在“SetRequestedSession”中进行处理。
	 *   检查我们是否能够立即加入所请求的会话（CanJoinRequestedSession）。如果可以，加入所请求的会话（JoinRequestedSession）。
	 *   如果不能，缓存所请求的会话，并指示游戏进入一个可以加入该会话的状态（ResetGameAndJoinRequestedSession）
	 *
	 *   
	 */
	/** Handles user accepting a session invite from an external source (for example, a platform overlay). Intended to be overridden per game. */
	/** 处理用户接受来自外部来源（例如，平台覆盖层）的会话邀请。旨在根据不同游戏进行重写。*/
	UE_API virtual void OnUserRequestedSession(const FPlatformUserId& PlatformUserId, UCommonSession_SearchResult* InRequestedSession, const FOnlineResultInformation& RequestedSessionResult);

	/** Handles OSS request that the session be destroyed */
	/** 处理有关销毁会话的 OSS 请求 */
	UE_API virtual void OnDestroySessionRequested(const FPlatformUserId& PlatformUserId, const FName& SessionName);

	/** Get the requested session */
	/** 获取所请求的会话 */
	UCommonSession_SearchResult* GetRequestedSession() const { return RequestedSession; }
	
	/** Set (or clear) the requested session. When this is set, the requested session flow begins. */
	/** 设置（或清除）所请求的会话。当此设置完成时，所请求的会话流程便开始启动。*/
	UE_API virtual void SetRequestedSession(UCommonSession_SearchResult* InRequestedSession);

	
	/** Checks if the requested session can be joined. Can be overridden per game. */
	/** 检查所请求的会话是否可以加入。此功能可针对每个游戏进行自定义设置。*/
	UE_API virtual bool CanJoinRequestedSession() const;

	
	/** Join the requested session */
	/** 加入所请求的会话 */
	UE_API virtual void JoinRequestedSession();

	
	/** Get the game into a state to join the requested session */
	/** 获取游戏状态，使其能够加入所请求的会话 */
	UE_API virtual void ResetGameAndJoinRequestedSession();

	// 重写父类方法 转发给游戏UI子系统创建根布局UI
	UE_API virtual int32 AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId) override;
	// 重写发u类方法 转发给游戏UI子系统
	UE_API virtual bool RemoveLocalPlayer(ULocalPlayer* ExistingPlayer) override;
	
	UE_API virtual void Init() override;
	UE_API virtual void ReturnToMainMenu() override;

private:
	/** This is the primary player*/
	/** 这是主要的玩家 */
	TWeakObjectPtr<ULocalPlayer> PrimaryPlayer;
	/** Session the player has requested to join */
	/** 玩家请求加入的会话 */
	UPROPERTY()
	TObjectPtr<UCommonSession_SearchResult> RequestedSession;
};

#undef UE_API
