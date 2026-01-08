// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "UObject/Interface.h"

#include "IGameSettingActionInterface.generated.h"

#define UE_API GAMESETTINGS_API

class UGameSetting;
class UObject;
struct FFrame;

UINTERFACE(MinimalAPI, meta = (BlueprintType))
class UGameSettingActionInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

// 特定游戏设置的行动 目前只在安全区用到了
class IGameSettingActionInterface
{
	GENERATED_BODY()

public:
	/** 执行游戏设置的行动 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UE_API bool ExecuteActionForSetting(FGameplayTag ActionTag, UGameSetting* InSetting);
};

#undef UE_API
