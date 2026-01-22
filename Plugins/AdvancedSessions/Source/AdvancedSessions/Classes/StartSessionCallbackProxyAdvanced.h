#pragma once

#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "StartSessionCallbackProxyAdvanced.generated.h"

UCLASS(MinimalAPI)
class UStartSessionCallbackProxyAdvanced : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()
	// 当会话成功开始时调用
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnSuccess;

	// 当开始会话发生错误时调用
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;

	/**
	 * 使用默认在线子系统开始会话。会话需要先通过调用 "CreateAdvancedSession" 节点创建。
	 * @param WorldContextObject 世界上下文对象
	 */
	UFUNCTION(
		BlueprintCallable
		, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject")
		, Category = "Online|AdvancedSessions"
	)
	static UStartSessionCallbackProxyAdvanced* StartAdvancedSession(UObject* WorldContextObject);

	// UOnlineBlueprintCallProxyBase 接口
	virtual void Activate() override;
	// UOnlineBlueprintCallProxyBase 接口结束

private:
	// 会话开始完成时的内部回调
	void OnStartCompleted(FName SessionName, bool bWasSuccessful);

	// 在线子系统执行的委托
	FOnStartSessionCompleteDelegate StartCompleteDelegate;

	// 上述注册委托的句柄
	FDelegateHandle StartCompleteDelegateHandle;

	// 此调用发生的世界上下文对象
	TWeakObjectPtr<UObject> WorldContextObject;
};