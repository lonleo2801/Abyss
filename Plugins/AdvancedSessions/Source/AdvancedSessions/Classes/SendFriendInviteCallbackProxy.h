// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Engine/LocalPlayer.h"
#include "SendFriendInviteCallbackProxy.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(AdvancedSendFriendInviteLog, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBlueprintSendFriendInviteDelegate);

UCLASS(MinimalAPI)
class USendFriendInviteCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// 当成功检索到好友列表时调用
	UPROPERTY(BlueprintAssignable)
	FBlueprintSendFriendInviteDelegate OnSuccess;

	// 当检索好友列表出错时调用
	UPROPERTY(BlueprintAssignable)
	FBlueprintSendFriendInviteDelegate OnFailure;

	// 添加使用定义的 UniqueNetId 的好友，某些接口现在不允许调用此函数（包括 STEAM）
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"), Category = "Online|AdvancedFriends")
	static USendFriendInviteCallbackProxy* SendFriendInvite(UObject* WorldContextObject, APlayerController *PlayerController, const FBPUniqueNetId &UniqueNetIDInvited);

	virtual void Activate() override;

private:
	// 检索好友列表时的内部回调
	void OnSendInviteComplete(int32 LocalPlayerNum, bool bWasSuccessful, const FUniqueNetId &InvitedPlayer, const FString &ListName, const FString &ErrorString);


	// 触发操作的玩家控制器
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	// 被邀请人的 UniqueNetID
	FBPUniqueNetId cUniqueNetId;

	// 完成时调用的委托
	FOnSendInviteComplete OnSendInviteCompleteDelegate;

	// 此调用发生的世界上下文对象
	TWeakObjectPtr<UObject> WorldContextObject;
};

