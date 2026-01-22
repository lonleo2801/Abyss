// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineUserInterface.h"
#include "Interfaces/OnlineMessageInterface.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "UObject/UObjectIterator.h"
#include "BlueprintDataDefinitions.h"
#include "AdvancedFriendsInterface.generated.h"


UINTERFACE(MinimalAPI)
class UAdvancedFriendsInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IAdvancedFriendsInterface
{
	GENERATED_IINTERFACE_BODY()
public:
	
	// 当指定的本地用户收到会话邀请时调用，在结果上使用 JoinSession 进行连接
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnSessionInviteReceived"))
	void OnSessionInviteReceived(FBPUniqueNetId PersonInviting, const FBlueprintSessionResult& SearchResult);

	// 当指定的本地用户已接受会话邀请时调用，在结果上使用 JoinSession 进行连接
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnSessionInviteAccepted"))
	void OnSessionInviteAccepted(FBPUniqueNetId PersonInvited, const FBlueprintSessionResult& SearchResult);

	// 当指定的本地用户已接受会话邀请时调用，在结果上使用 JoinSession 进行连接
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnPlayerVoiceStateChanged"))
	void OnPlayerVoiceStateChanged(FBPUniqueNetId PlayerId, bool bIsTalking);

	// 当指定的本地用户更改登录状态时调用
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnPlayerLoginChanged"))
	void OnPlayerLoginChanged(int32 PlayerNum);

	// 当指定的本地用户更改登录状态时调用
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnPlayerLoginStatusChanged"))
	void OnPlayerLoginStatusChanged(EBPLoginStatus PreviousStatus, EBPLoginStatus NewStatus, FBPUniqueNetId PlayerUniqueNetID);

	// REMOVED BECAUSE IT WAS NEVER BEING CALLED
	// Called when the designated LocalUser has received a session invite, use JoinSession on result to connect
	//UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "OnSessionInviteReceived"))
	//void OnSessionInviteReceived(const FBPUniqueNetId &FriendInviting, const FBlueprintSessionResult &Session);

};	
