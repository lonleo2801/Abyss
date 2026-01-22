// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "FindSessionsCallbackProxy.h"
#include "BlueprintDataDefinitions.h"
#include "FindSessionsCallbackProxyAdvanced.generated.h"


FORCEINLINE bool operator==(const FBlueprintSessionResult& A, const FBlueprintSessionResult& B)
{
	return (A.OnlineResult.IsValid() == B.OnlineResult.IsValid() && (A.OnlineResult.GetSessionIdStr() == B.OnlineResult.GetSessionIdStr()));
}

UCLASS(MinimalAPI)
class UFindSessionsCallbackProxyAdvanced : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// 当查询成功时调用
	UPROPERTY(BlueprintAssignable)
	FBlueprintFindSessionsResultDelegate OnSuccess;

	// 当查询失败时调用
	UPROPERTY(BlueprintAssignable)
	FBlueprintFindSessionsResultDelegate OnFailure;

	// 使用默认在线子系统搜索已发布的会话，并包含一个过滤器数组
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm="Filters"), Category = "Online|AdvancedSessions")
	static UFindSessionsCallbackProxyAdvanced* FindSessionsAdvanced(UObject* WorldContextObject, class APlayerController* PlayerController, int32 MaxResults, bool bUseLAN, EBPServerPresenceSearchType ServerTypeToSearch, const TArray<FSessionsSearchSetting> &Filters, bool bEmptyServersOnly = false, bool bNonEmptyServersOnly = false, bool bSecureServersOnly = false, /*bool bSearchLobbies = true,*/ int MinSlotsAvailable = 0);

	static bool CompareVariants(const FVariantData &A, const FVariantData &B, EOnlineComparisonOpRedux Comparator);
	
	// 通过给定的搜索参数过滤会话结果数组，返回包含过滤结果的新数组
	UFUNCTION(BluePrintCallable, meta = (Category = "Online|AdvancedSessions"))
	static void FilterSessionResults(const TArray<FBlueprintSessionResult> &SessionResults, const TArray<FSessionsSearchSetting> &Filters, TArray<FBlueprintSessionResult> &FilteredResults);
	
	// 已移除，FindSessionsCallbackProxy 中默认内置版本工作正常
	/*UFUNCTION(BlueprintPure, Category = "Online|Session")
	static int32 GetPingInMs(const FBlueprintSessionResult& Result);

	UFUNCTION(BlueprintPure, Category = "Online|Session")
	static FString GetServerName(const FBlueprintSessionResult& Result);

	UFUNCTION(BlueprintPure, Category = "Online|Session")
	static int32 GetCurrentPlayers(const FBlueprintSessionResult& Result);

	UFUNCTION(BlueprintPure, Category = "Online|Session")
	static int32 GetMaxPlayers(const FBlueprintSessionResult& Result);*/


	// UOnlineBlueprintCallProxyBase 接口
	virtual void Activate() override;
	// UOnlineBlueprintCallProxyBase 接口结束

private:
	// 会话搜索完成时的内部回调，调用公共成功/失败回调
	void OnCompleted(bool bSuccess);

	bool bRunSecondSearch;
	bool bIsOnSecondSearch;

	TArray<FBlueprintSessionResult> SessionSearchResults;

private:
	// 触发操作的玩家控制器
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	// 在线子系统执行的委托
	FOnFindSessionsCompleteDelegate Delegate;

	// 注册的 OnFindSessionsComplete 委托句柄
	FDelegateHandle DelegateHandle;

	// 用于跟踪搜索结果的对象
	TSharedPtr<FOnlineSessionSearch> SearchObject;
	TSharedPtr<FOnlineSessionSearch> SearchObjectDedicated;

	// 是否搜索局域网
	bool bUseLAN;

	// 是否搜索专用服务器
	EBPServerPresenceSearchType ServerSearchType;

	// 返回的最大结果数
	int MaxResults;

	// 存储额外设置
	TArray<FSessionsSearchSetting> SearchSettings;

	// 仅搜索空服务器
	bool bEmptyServersOnly;

	// 仅搜索非空服务器
	bool bNonEmptyServersOnly;

	// 仅搜索安全服务器
	bool bSecureServersOnly;

	// 搜索大厅
	//bool bSearchLobbies;

	// 搜索所需的最小可用插槽
	int MinSlotsAvailable;

	// 此调用发生的世界上下文对象
	TWeakObjectPtr<UObject> WorldContextObject;
};
