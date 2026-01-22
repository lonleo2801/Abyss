// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "GetUserPrivilegeCallbackProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBlueprintGetUserPrivilegeDelegate,/* const &FBPUniqueNetId, PlayerID,*/ EBPUserPrivileges, QueriedPrivilege, bool, HadPrivilege);

UCLASS(MinimalAPI)
class UGetUserPrivilegeCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// 当销毁成功时调用
	UPROPERTY(BlueprintAssignable)
	FBlueprintGetUserPrivilegeDelegate OnSuccess;

	// 当销毁失败时调用
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;

	// 获取用户权限
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"), Category = "Online|AdvancedIdentity")
	static UGetUserPrivilegeCallbackProxy* GetUserPrivilege(UObject* WorldContextObject, const EBPUserPrivileges & PrivilegeToCheck, const FBPUniqueNetId & PlayerUniqueNetID);

	// UOnlineBlueprintCallProxyBase 接口
	virtual void Activate() override;
	// UOnlineBlueprintCallProxyBase 接口结束

private:
	// 操作完成时的内部回调，调用公共成功/失败回调
	void OnCompleted(const FUniqueNetId& PlayerID, EUserPrivileges::Type Privilege, uint32 Result);

private:
	// 触发操作的玩家控制器
	FBPUniqueNetId PlayerUniqueNetID;

	// 要检查的权限
	EBPUserPrivileges UserPrivilege;

	// 此调用发生的世界上下文对象
	TWeakObjectPtr<UObject> WorldContextObject;
};
