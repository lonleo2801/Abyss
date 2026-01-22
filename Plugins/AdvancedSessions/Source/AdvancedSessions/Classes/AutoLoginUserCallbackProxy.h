// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Engine/LocalPlayer.h"
#include "AutoLoginUserCallbackProxy.generated.h"

UCLASS(MinimalAPI)
class UAutoLoginUserCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// 当销毁成功时调用
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnSuccess;

	// 当销毁失败时调用
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;

	/**
	 * 使用在命令行上传递的参数将玩家登录到在线服务。
	 * 期望 -AUTH_LOGIN=<UserName> -AUTH_PASSWORD=<password>。如果任一
	 * 缺失，该函数返回 false 且不开始登录过程。
	 *
	 * @param LocalUserNum 关联用户的控制器编号
	 *
	 */
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"), Category = "Online|AdvancedIdentity")
	static UAutoLoginUserCallbackProxy* AutoLoginUser(UObject* WorldContextObject, int32 LocalUserNum);

	// UOnlineBlueprintCallProxyBase 接口
	virtual void Activate() override;
	// UOnlineBlueprintCallProxyBase 接口结束

private:
	// 操作完成时的内部回调，调用公共成功/失败回调
	void OnCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& ErrorVal);

private:
	// 关联用户的控制器编号
	int32 LocalUserNumber;

	// 在线子系统执行的委托
	FOnLoginCompleteDelegate Delegate;

	// 注册的 OnDestroySessionComplete 委托句柄
	FDelegateHandle DelegateHandle;

	// 此调用发生的世界上下文对象
	TWeakObjectPtr<UObject> WorldContextObject;
};
