// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "BlueprintDataDefinitions.h"
#include "CancelFindSessionsCallbackProxy.generated.h"

UCLASS(MinimalAPI)
class UCancelFindSessionsCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// 当销毁成功时调用
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnSuccess;

	// 当销毁失败时调用
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;

	// 取消查找会话
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"), Category = "Online|AdvancedSessions")
	static UCancelFindSessionsCallbackProxy* CancelFindSessions(UObject* WorldContextObject, class APlayerController* PlayerController);

	// UOnlineBlueprintCallProxyBase 接口
	virtual void Activate() override;
	// UOnlineBlueprintCallProxyBase 接口结束

private:
	// 操作完成时的内部回调，调用公共成功/失败回调
	void OnCompleted(bool bWasSuccessful);

private:
	// 触发操作的玩家控制器
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	// 在线子系统执行的委托
	FOnCancelFindSessionsCompleteDelegate Delegate;

	// 注册的 OnDestroySessionComplete 委托句柄
	FDelegateHandle DelegateHandle;

	// 此调用发生的世界上下文对象
	TWeakObjectPtr<UObject> WorldContextObject;
};
