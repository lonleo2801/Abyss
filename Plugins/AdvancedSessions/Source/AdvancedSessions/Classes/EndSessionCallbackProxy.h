// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "BlueprintDataDefinitions.h"
#include "EndSessionCallbackProxy.generated.h"

UCLASS(MinimalAPI)
class UEndSessionCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// 当销毁成功时调用
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnSuccess;

	// 当销毁失败时调用
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;

	/**
	 *	  结束当前会话。一般来说，对于几乎所有用途，您应该使用引擎的原生 Destroy Session 节点。
	 *	  这个节点主要用于使用 StartSession 并可选地手动管理会话状态的人。
	 */
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"), Category = "Online|AdvancedSessions")
	static UEndSessionCallbackProxy* EndSession(UObject* WorldContextObject, class APlayerController* PlayerController);

	// UOnlineBlueprintCallProxyBase 接口
	virtual void Activate() override;
	// UOnlineBlueprintCallProxyBase 接口结束

private:
	// 操作完成时的内部回调，调用公共成功/失败回调
	void OnCompleted(FName SessionName, bool bWasSuccessful);

private:
	// 触发操作的玩家控制器
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	// 在线子系统执行的委托
	FOnEndSessionCompleteDelegate Delegate;

	// 注册的 OnDestroySessionComplete 委托句柄
	FDelegateHandle DelegateHandle;

	// 此调用发生的世界上下文对象
	TWeakObjectPtr<UObject> WorldContextObject;
};
