// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameSettingValue.h"

#include "GameSettingValueDiscrete.generated.h"

#define UE_API GAMESETTINGS_API

class UObject;
struct FFrame;

UCLASS(MinimalAPI, Abstract)
class UGameSettingValueDiscrete : public UGameSettingValue
{
	GENERATED_BODY()

public:
	UE_API UGameSettingValueDiscrete();

	/** UGameSettingValueDiscrete */
	// 设置分离选项的索引
	UE_API virtual void SetDiscreteOptionByIndex(int32 Index) PURE_VIRTUAL(,);

	// 获取分离选项的索引
	UFUNCTION(BlueprintCallable)
	UE_API virtual int32 GetDiscreteOptionIndex() const PURE_VIRTUAL(,return INDEX_NONE;);

	/** Optional */
	// 可惜 默认分离选项的索引
	UFUNCTION(BlueprintCallable)
	virtual int32 GetDiscreteOptionDefaultIndex() const { return INDEX_NONE; }

	// 获取所有的选项文本
	UFUNCTION(BlueprintCallable)
	UE_API virtual TArray<FText> GetDiscreteOptions() const PURE_VIRTUAL(,return TArray<FText>(););

	// 获取分解后的值
	UE_API virtual FString GetAnalyticsValue() const;
};

#undef UE_API
