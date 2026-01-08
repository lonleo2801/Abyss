// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "LoadingProcessInterface.h"
#include "UObject/Object.h"

#include "LoadingProcessTask.generated.h"

#define UE_API COMMONLOADINGSCREEN_API

struct FFrame;

// 创建一个接口实例,用来临时显示加载屏幕.
// 注意一定要持有这个对象,防止GC
// 和异步蓝图节点的写法很像.
UCLASS(MinimalAPI, BlueprintType)
class ULoadingProcessTask : public UObject, public ILoadingProcessInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, meta=(WorldContext = "WorldContextObject"))
	static UE_API ULoadingProcessTask* CreateLoadingScreenProcessTask(UObject* WorldContextObject, const FString& ShowLoadingScreenReason);

public:
	ULoadingProcessTask() { }

	UFUNCTION(BlueprintCallable)
	UE_API void Unregister();

	UFUNCTION(BlueprintCallable)
	UE_API void SetShowLoadingScreenReason(const FString& InReason);

	UE_API virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	
	FString Reason;
};

#undef UE_API
