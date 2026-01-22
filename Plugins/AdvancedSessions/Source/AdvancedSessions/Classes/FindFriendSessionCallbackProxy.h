// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Engine/LocalPlayer.h"
#include "FindFriendSessionCallbackProxy.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(AdvancedFindFriendSessionLog, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBlueprintFindFriendSessionDelegate, const TArray<FBlueprintSessionResult> &, SessionInfo);

UCLASS(MinimalAPI)
class UFindFriendSessionCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// 当成功检索到好友列表时调用
	UPROPERTY(BlueprintAssignable)
	FBlueprintFindFriendSessionDelegate OnSuccess;

	// 当检索好友列表出错时调用
	UPROPERTY(BlueprintAssignable)
	FBlueprintFindFriendSessionDelegate OnFailure;

	// 尝试获取好友所在的当前会话
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"), Category = "Online|AdvancedFriends")
	static UFindFriendSessionCallbackProxy* FindFriendSession(UObject* WorldContextObject, APlayerController *PlayerController, const FBPUniqueNetId &FriendUniqueNetId);

	virtual void Activate() override;

private:
	// 检索好友列表时的内部回调
	void OnFindFriendSessionCompleted(int32 LocalPlayer, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& SessionInfo);

	// 触发操作的玩家控制器
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	// 被邀请人的 UniqueNetID
	FBPUniqueNetId cUniqueNetId;

	// 完成时调用的委托
	FOnFindFriendSessionCompleteDelegate OnFindFriendSessionCompleteDelegate;

	// 上述注册委托的句柄
	FDelegateHandle FindFriendSessionCompleteDelegateHandle;

	// 此调用发生的世界上下文对象
	TWeakObjectPtr<UObject> WorldContextObject;
};

