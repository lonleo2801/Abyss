// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Engine/LocalPlayer.h"
#include "LoginUserCallbackProxy.generated.h"

UCLASS(MinimalAPI)
class ULoginUserCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// 当销毁成功时调用
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnSuccess;

	// 当销毁失败时调用
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;

	// 登录身份接口
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject", AdvancedDisplay = "AuthType"), Category = "Online|AdvancedIdentity")
	static ULoginUserCallbackProxy* LoginUser(UObject* WorldContextObject, class APlayerController* PlayerController, FString UserID, FString UserToken, FString AuthType);

	// UOnlineBlueprintCallProxyBase 接口
	virtual void Activate() override;
	// UOnlineBlueprintCallProxyBase 接口结束

private:
	// 操作完成时的内部回调，调用公共成功/失败回调
	void OnCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& ErrorVal);

private:
	// 触发操作的玩家控制器
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	// 用户ID
	FString UserID;

	// 用户密码/令牌
	FString UserToken;

	// 认证类型
	FString AuthType;

	// 在线子系统执行的委托
	FOnLoginCompleteDelegate Delegate;

	// 注册的 OnDestroySessionComplete 委托句柄
	FDelegateHandle DelegateHandle;

	// 此调用发生的世界上下文对象
	TWeakObjectPtr<UObject> WorldContextObject;
};
