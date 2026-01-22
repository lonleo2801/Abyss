// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "BlueprintDataDefinitions.h"
#include "CreateSessionCallbackProxyAdvanced.generated.h"

UCLASS(MinimalAPI)
class UCreateSessionCallbackProxyAdvanced : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// 当会话创建成功时调用
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnSuccess;

	// 当会话创建失败时调用
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;

	/**
	 *    使用默认在线子系统创建会话，并提供高级可选输入。对于专用服务器，请将 UsePresence 设为 false 并将 IsDedicatedServer 设为 true。专用服务器不使用 Presence。
	 *    @param PublicConnections	当做 'listen' 服务器时，必须 >=2（ListenServer 本身算作一个连接）
	 *    @param bUseLAN			当你想玩局域网时，必须使用选项 'bIsLanMatch' 加载关卡
	 *	  @param bUseLobbiesIfAvailable 用于标记子系统使用大厅 API 而不是通用托管（如果 API 支持），通常在 Steam 上 Listen 服务器为 true，专用服务器为 false
	 *	对于 'listen' 服务器必须为 true（必须使用选项 'listen' 加载地图），对于 'dedicated' 服务器为 false。
	 *	  @param bShouldAdvertise	设为 true 时，在线子系统将在有人搜索服务器时列出您的服务器。否则服务器将被隐藏，只能通过邀请加入。
	 *	  @param bUseLobbiesVoiceChatIfAvailable 设为 true 以设置语音聊天大厅（如果 API 支持）
	 * 	  @param bStartAfterCreate 设为 true 以在创建后启动会话。如果为 false，则需要在准备好后手动调用 StartSession。
	 */
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject",AutoCreateRefTerm="ExtraSettings"), Category = "Online|AdvancedSessions")
		static UCreateSessionCallbackProxyAdvanced* CreateAdvancedSession(UObject* WorldContextObject, const TArray<FSessionPropertyKeyPair>& ExtraSettings, class APlayerController* PlayerController = NULL, int32 PublicConnections = 100, int32 PrivateConnections = 0, bool bUseLAN = false, bool bAllowInvites = true, bool bIsDedicatedServer = false, /*bool bUsePresence = true,*/ bool bUseLobbiesIfAvailable = true, bool bAllowJoinViaPresence = true, bool bAllowJoinViaPresenceFriendsOnly = false, bool bAntiCheatProtected = false, bool bUsesStats = false, bool bShouldAdvertise = true, bool bUseLobbiesVoiceChatIfAvailable = false, bool bStartAfterCreate = true);

	// UOnlineBlueprintCallProxyBase 接口
	virtual void Activate() override;
	// UOnlineBlueprintCallProxyBase 接口结束

private:
	// 会话创建完成时的内部回调，可选调用 StartSession
	void OnCreateCompleted(FName SessionName, bool bWasSuccessful);

	// 会话启动完成时的内部回调
	void OnStartCompleted(FName SessionName, bool bWasSuccessful);

	// 触发操作的玩家控制器
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	// 在线子系统执行的委托
	FOnCreateSessionCompleteDelegate CreateCompleteDelegate;

	// 在线子系统执行的委托
	FOnStartSessionCompleteDelegate StartCompleteDelegate;

	// 上述注册委托的句柄
	FDelegateHandle CreateCompleteDelegateHandle;
	FDelegateHandle StartCompleteDelegateHandle;

	// 公共连接数
	int NumPublicConnections;

	// 私有连接数
	int NumPrivateConnections;

	// 是否搜索局域网
	bool bUseLAN;

	// 是否允许邀请
	bool bAllowInvites;

	// 是否为专用服务器
	bool bDedicatedServer;

	// 是否使用 presence 选项
	bool bUsePresence;

	// 如果 API 支持，是否优先使用大厅进行托管
	bool bUseLobbiesIfAvailable;

	// 是否允许通过 presence 加入
	bool bAllowJoinViaPresence;

	// 仅允许好友通过 presence 加入
	bool bAllowJoinViaPresenceFriendsOnly;

	// 声明服务器受反作弊保护
	bool bAntiCheatProtected;

	// 记录统计数据
	bool bUsesStats;

	// 是否应该广播服务器？
	bool bShouldAdvertise;

	// 如果 API 支持，是否优先使用语音聊天大厅
	bool bUseLobbiesVoiceChatIfAvailable;

	// 是否在创建后自动启动会话
	bool bStartAfterCreate;

	// 存储额外设置
	TArray<FSessionPropertyKeyPair> ExtraSettings;

	// 此调用发生的世界上下文对象
	TWeakObjectPtr<UObject> WorldContextObject;
};

