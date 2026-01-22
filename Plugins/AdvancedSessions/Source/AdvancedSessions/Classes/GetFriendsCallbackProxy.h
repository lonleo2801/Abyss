// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Engine/LocalPlayer.h"
#include "GetFriendsCallbackProxy.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(AdvancedGetFriendsLog, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBlueprintGetFriendsListDelegate, const TArray<FBPFriendInfo>&, Results);

UCLASS(MinimalAPI)
class UGetFriendsCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// 当成功检索到好友列表时调用
	UPROPERTY(BlueprintAssignable)
	FBlueprintGetFriendsListDelegate OnSuccess;

	// 当检索好友列表出错时调用
	UPROPERTY(BlueprintAssignable)
	FBlueprintGetFriendsListDelegate OnFailure;

	// 从在线子系统获取玩家好友列表并返回，稍后可以通过 GetStoredFriendsList 检索
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"), Category = "Online|AdvancedFriends")
	static UGetFriendsCallbackProxy* GetAndStoreFriendsList(UObject* WorldContextObject, class APlayerController* PlayerController);

	virtual void Activate() override;

private:
	// 检索好友列表时的内部回调
	void OnReadFriendsListCompleted(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorString);

	// 触发操作的玩家控制器
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	// 执行的委托
	FOnReadFriendsListComplete FriendListReadCompleteDelegate;

	// 要获取的好友列表类型
	// 已移除，因为除 Facebook 接口外，其他接口目前甚至不支持除默认好友列表之外的任何内容。
	//EBPFriendsLists FriendListToGet;

	// 此调用发生的世界上下文对象
	TWeakObjectPtr<UObject> WorldContextObject;
};

