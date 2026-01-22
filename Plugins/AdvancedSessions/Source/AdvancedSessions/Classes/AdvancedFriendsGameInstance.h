// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "BlueprintDataDefinitions.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineUserInterface.h"
#include "Interfaces/OnlineMessageInterface.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "UObject/UObjectIterator.h"
#include "AdvancedFriendsInterface.h"

#include "AdvancedFriendsGameInstance.generated.h"


// 一般高级好友接口日志
DECLARE_LOG_CATEGORY_EXTERN(AdvancedFriendsInterfaceLog, Log, All);

UCLASS()
class ADVANCEDSESSIONS_API UAdvancedFriendsGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	UAdvancedFriendsGameInstance(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdvancedFriendsInterface)
	bool bCallFriendInterfaceEventsOnPlayerControllers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdvancedFriendsInterface)
	bool bCallIdentityInterfaceEventsOnPlayerControllers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdvancedFriendsInterface)
	bool bCallVoiceInterfaceEventsOnPlayerControllers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdvancedVoiceInterface)
	bool bEnableTalkingStatusDelegate;

	// 如果为真，我们将自动加入我们在覆盖界面中接受的会话。
	// 这可能会妨碍 Beacon 会话，您可能希望禁用它。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdvancedFriendsInterface)
	bool bAutoJoinSessionOnAcceptedUserInviteReceived = false;

	// 如果为真，我们将收到邀请时自动漫游到游戏会话。
	// 这可能会妨碍 Beacon 会话，您可能希望禁用它。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdvancedFriendsInterface)
	bool bAutoTravelOnAcceptedUserInviteReceived = false;

	//virtual void PostLoad() override;
	virtual void Shutdown() override;
	virtual void Init() override;

	//*** 本地收到会话邀请 ***//
	FOnSessionInviteReceivedDelegate SessionInviteReceivedDelegate;
	FDelegateHandle SessionInviteReceivedDelegateHandle;

	// 从 Steam UI "加入游戏" 直接加入的自定义句柄
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	// 自定义 Steam UI 加入用户函数 #自身邀请#
	void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult);
	// 自定义 Steam UI 客户端漫游函数 #自身邀请#
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	//const FUniqueNetId& /*UserId*/, const FUniqueNetId& /*FromId*/, const FString& /*AppId*/, const FOnlineSessionSearchResult& /*InviteResult*/
	void OnSessionInviteReceivedMaster(const FUniqueNetId & PersonInvited, const FUniqueNetId & PersonInviting, const FString & AppId, const FOnlineSessionSearchResult& SessionToJoin);

	// 本地玩家接受会话邀请后触发此事件，对会话结果调用 JoinSession 以加入
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedFriends")
	void OnSessionInviteReceived(int32 LocalPlayerNum, FBPUniqueNetId PersonInviting, const FString& AppId, const FBlueprintSessionResult& SessionToJoin);

	//*** 本地接受会话邀请 ***//
	FOnSessionUserInviteAcceptedDelegate SessionInviteAcceptedDelegate;
	FDelegateHandle SessionInviteAcceptedDelegateHandle;

	void OnSessionInviteAcceptedMaster(const bool bWasSuccessful, int32 LocalPlayer, TSharedPtr<const FUniqueNetId> PersonInviting, const FOnlineSessionSearchResult& SessionToJoin);

	// 本地玩家接受会话邀请后触发此事件，对会话结果调用 JoinSession 以加入
	// 此函数目前未挂接到任何 Epics 默认子系统，它是为自定义子系统准备的
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedFriends")
	void OnSessionInviteAccepted(int32 LocalPlayerNum, FBPUniqueNetId PersonInvited, const FBlueprintSessionResult& SessionToJoin);


	// 如果 bEnableTalkingStatusDelegate 属性为真，则语音状态更改后触发此事件
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedVoice")
	void OnPlayerTalkingStateChanged(FBPUniqueNetId PlayerId, bool bIsTalking);

	void OnPlayerTalkingStateChangedMaster(TSharedRef<const FUniqueNetId> PlayerId, bool bIsTalking);

	FOnPlayerTalkingStateChangedDelegate PlayerTalkingStateChangedDelegate;
	FDelegateHandle PlayerTalkingStateChangedDelegateHandle;


	// 当指定的本地用户更改登录状态时调用
	UFUNCTION(BlueprintImplementableEvent , Category = "AdvancedIdentity", meta = (DisplayName = "OnPlayerLoginChanged"))
	void OnPlayerLoginChanged(int32 PlayerNum);

	void OnPlayerLoginChangedMaster(int32 PlayerNum);
	FOnLoginChangedDelegate PlayerLoginChangedDelegate;
	FDelegateHandle PlayerLoginChangedDelegateHandle;

	// 当指定的本地用户更改登录状态时调用
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedIdentity", meta = (DisplayName = "OnPlayerLoginStatusChanged"))
	void OnPlayerLoginStatusChanged(int32 PlayerNum, EBPLoginStatus PreviousStatus, EBPLoginStatus NewStatus, FBPUniqueNetId NewPlayerUniqueNetID);

	void OnPlayerLoginStatusChangedMaster(int32 PlayerNum, ELoginStatus::Type PreviousStatus, ELoginStatus::Type NewStatus, const FUniqueNetId & NewPlayerUniqueNetID);
	FOnLoginStatusChangedDelegate PlayerLoginStatusChangedDelegate;
	FDelegateHandle PlayerLoginStatusChangedDelegateHandle;


	//*** Session Invite Received From Friend ***//
	// REMOVED BECAUSE IT NEVER GETS CALLED
	/*FOnSessionInviteReceivedDelegate SessionInviteReceivedDelegate;
	FDelegateHandle SessionInviteReceivedDelegateHandle;

	void OnSessionInviteReceivedMaster(const FUniqueNetId &InvitedPlayer, const FUniqueNetId &FriendInviting, const FOnlineSessionSearchResult& Session);

	// After a session invite has been sent from a friend this event is triggered, call JoinSession on the session result to join it
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedFriends")
	void OnSessionInviteReceived(const FBPUniqueNetId &InvitedPlayer, const FBPUniqueNetId &FriendInviting, const FBlueprintSessionResult &Session);
	*/

	//*** Friend Invite Accepted ***//
	/*FOnInviteAcceptedDelegate FriendInviteAcceptedDelegate;
	FDelegateHandle FriendInviteAcceptedDelegateHandle;

	void OnFriendInviteAcceptedDelegateMaster(const FUniqueNetId& LocalPlayer, const FUniqueNetId &PlayerInvited);

	// After a session invite has been accepted by a friend this event is triggered
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedFriends")
	void OnFriendInviteAccepted(const FBPUniqueNetId &InvitedPlayer, const FBPUniqueNetId &PlayerInvited);
	*/

	//*** Friend Invite Rejected ***//
	/*FOnInviteRejectedDelegate SessionInviteRejectedByFriendDelegate;
	FDelegateHandle InviteRejectedByFriendDelegateHandle;

	void OnFriendInviteRejectedDelegateMaster(const FUniqueNetId& LocalPlayer, const FUniqueNetId &PlayerDeclined);

	// After a friend invite has been rejected this event is triggered
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedFriends")
	void OnFriendInviteRejected(const FBPUniqueNetId &InvitedPlayer, const FBPUniqueNetId &PlayerDeclined);
	*/

	//*** Removed By Friend ***//
	/*FOnFriendRemovedDelegate RemovedByFriendDelegate;
	FDelegateHandle RemovedByFriendDelegateHandle;

	void OnRemovedByFriendDelegateMaster(const FUniqueNetId& LocalPlayer, const FUniqueNetId &FriendRemoved);

	// After a friend removed the player this event is triggered
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedFriends")
	void OnRemovedByFriend(const FBPUniqueNetId &InvitedPlayer, const FBPUniqueNetId &FriendRemoved);*/
};

