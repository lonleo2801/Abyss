// Copyright Epic Games, Inc. All Rights Reserved.
// Finished.
#pragma once

#include "Blueprint/UserWidget.h"

#include "GameSettingDetailExtension.generated.h"

#define UE_API GAMESETTINGS_API

enum class EGameSettingChangeReason : uint8;

class UGameSetting;
class UObject;

/**
 * 游戏设置对应的细节面板拓展控件的父类!
 */
UCLASS(MinimalAPI, Abstract, meta = (Category = "Settings", DisableNativeTick))
class UGameSettingDetailExtension : public UUserWidget
{
	GENERATED_BODY()

public:
	// 设置游戏设置
	UE_API void SetSetting(UGameSetting* InSetting);
	
protected:
	// 绑定游戏设置变动的代理
	UE_API virtual void NativeSettingAssigned(UGameSetting* InSetting);
	// 传递 当游戏设置的值发生改变
	UE_API virtual void NativeSettingValueChanged(UGameSetting* InSetting, EGameSettingChangeReason Reason);

	// 蓝图拓展 当游戏设置被指定后
	UFUNCTION(BlueprintImplementableEvent)
	UE_API void OnSettingAssigned(UGameSetting* InSetting);

	// 蓝图拓展 当游戏设置的值发生改变之后
	UFUNCTION(BlueprintImplementableEvent)
	UE_API void OnSettingValueChanged(UGameSetting* InSetting);

protected:
	UPROPERTY(Transient)
	TObjectPtr<UGameSetting> Setting;
};

#undef UE_API
