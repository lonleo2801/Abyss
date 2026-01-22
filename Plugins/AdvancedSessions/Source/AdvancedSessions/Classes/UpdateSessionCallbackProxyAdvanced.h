// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "BlueprintDataDefinitions.h"
#include "UpdateSessionCallbackProxyAdvanced.generated.h"

UCLASS(MinimalAPI)
class UUpdateSessionCallbackProxyAdvanced : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// 当会话更新成功时调用
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnSuccess;

	// 当会话更新失败时调用
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;

	// 使用默认在线子系统更新会话，带有高级可选输入，必须填写所有类别，否则将传入您不想要的默认值
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject",AutoCreateRefTerm="ExtraSettings"), Category = "Online|AdvancedSessions")
	static UUpdateSessionCallbackProxyAdvanced* UpdateSession(UObject* WorldContextObject, const TArray<FSessionPropertyKeyPair> &ExtraSettings, int32 PublicConnections = 100, int32 PrivateConnections = 0, bool bUseLAN = false, bool bAllowInvites = false, bool bAllowJoinInProgress = false, bool bRefreshOnlineData = true, bool bIsDedicatedServer = false, bool bShouldAdvertise = true, bool bAllowJoinViaPresence = true, bool bAllowJoinViaPresenceFriendsOnly = false);

	// UOnlineBlueprintCallProxyBase 接口
	virtual void Activate() override;
	// UOnlineBlueprintCallProxyBase 接口结束

private:
	// 会话创建完成时的内部回调，调用 StartSession
	void OnUpdateCompleted(FName SessionName, bool bWasSuccessful);

	// 在线子系统执行的委托
	FOnUpdateSessionCompleteDelegate OnUpdateSessionCompleteDelegate;

	// 上述注册委托的句柄
	FDelegateHandle OnUpdateSessionCompleteDelegateHandle;

	// 公共连接数
	int NumPublicConnections = 100;

	// 私有连接数
	int NumPrivateConnections = 0;

	// 是否搜索局域网
	bool bUseLAN = false;

	// 是否允许邀请
	bool bAllowInvites = true;

	// 存储额外设置
	TArray<FSessionPropertyKeyPair> ExtraSettings;

	// 是否更新在线数据
	bool bRefreshOnlineData = true;

	// 允许中途加入
	bool bAllowJoinInProgress = true;

	// 允许通过 presence 加入
	bool bAllowJoinViaPresence = true;

	// 仅允许好友通过 presence 加入
	bool bAllowJoinViaPresenceFriendsOnly = false;

	// 更新是否为专用服务器
	bool bDedicatedServer = false;

	// 是否应该广播
	bool bShouldAdvertise = true;

	// 此调用发生的世界上下文对象
	TWeakObjectPtr<UObject> WorldContextObject;
};

