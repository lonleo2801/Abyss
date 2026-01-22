// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "GetRecentPlayersCallbackProxy.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(AdvancedGetRecentPlayersLog, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBlueprintGetRecentPlayersDelegate, const TArray<FBPOnlineRecentPlayer>&, Results);

UCLASS(MinimalAPI)
class UGetRecentPlayersCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// 当成功检索到好友列表时调用
	UPROPERTY(BlueprintAssignable)
	FBlueprintGetRecentPlayersDelegate OnSuccess;

	// 当检索好友列表出错时调用
	UPROPERTY(BlueprintAssignable)
	FBlueprintGetRecentPlayersDelegate OnFailure;

	// 从在线子系统获取最近玩家列表并返回，稍后可以通过 GetStoredRecentPlayersList 检索，如果未找到最近玩家可能会失败
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"), Category = "Online|AdvancedFriends")
	static UGetRecentPlayersCallbackProxy* GetAndStoreRecentPlayersList(UObject* WorldContextObject, const FBPUniqueNetId &UniqueNetId);

	virtual void Activate() override;

private:
	// 检索好友列表时的内部回调
	void OnQueryRecentPlayersCompleted(const FUniqueNetId &UserID, const FString &Namespace, bool bWasSuccessful, const FString& ErrorString);
	// 注册的 OnFindSessionsComplete 委托句柄
	FDelegateHandle DelegateHandle;

	// 触发操作的玩家控制器
	//TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	// 要获取最近玩家的人的 UniqueNetID
	FBPUniqueNetId cUniqueNetId;

	// 执行的委托
	FOnQueryRecentPlayersCompleteDelegate QueryRecentPlayersCompleteDelegate;

	// 此调用发生的世界上下文对象
	TWeakObjectPtr<UObject> WorldContextObject;
};

