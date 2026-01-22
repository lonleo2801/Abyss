// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Online.h"
#include "Engine/LocalPlayer.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineUserInterface.h"
#include "Interfaces/OnlineMessageInterface.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "UObject/UObjectIterator.h"

#include "AdvancedFriendsLibrary.generated.h"


// 一般高级会话日志
DECLARE_LOG_CATEGORY_EXTERN(AdvancedFriendsLog, Log, All);

UCLASS()
class UAdvancedFriendsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	//********* 好友列表函数 *************//

	// 向当前在线会话发送邀请给好友列表
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|FriendsList", meta = (ExpandEnumAsExecs = "Result"))
	static void SendSessionInviteToFriends(APlayerController *PlayerController, const TArray<FBPUniqueNetId> &Friends, EBlueprintResultSwitch &Result);

	// 向当前在线会话发送邀请给一个好友
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|FriendsList", meta = (ExpandEnumAsExecs = "Result"))
	static void SendSessionInviteToFriend(APlayerController *PlayerController, const FBPUniqueNetId &FriendUniqueNetId, EBlueprintResultSwitch &Result);

	// 从先前读取/保存的好友列表中获取一个好友（必须先调用 GetFriends 此函数才能返回内容）
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|FriendsList")
	static void GetFriend(APlayerController *PlayerController, const FBPUniqueNetId FriendUniqueNetId, FBPFriendInfo &Friend);

	// 获取先前读取/保存的好友列表（必须先调用 GetFriends 此函数才能返回内容）
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|FriendsList")
	static void GetStoredFriendsList(APlayerController *PlayerController, TArray<FBPFriendInfo> &FriendsList);

	// 获取先前读取/保存的最近玩家列表（必须先调用 GetRecentPlayers 此函数才能返回内容）
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|RecentPlayersList")
	static void GetStoredRecentPlayersList(FBPUniqueNetId UniqueNetId, TArray<FBPOnlineRecentPlayer> &PlayersList);

	// 检查 UniqueNetId 是否为好友
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedFriends|FriendsList")
	static void IsAFriend(APlayerController *PlayerController, const FBPUniqueNetId UniqueNetId, bool &IsFriend);
};	
