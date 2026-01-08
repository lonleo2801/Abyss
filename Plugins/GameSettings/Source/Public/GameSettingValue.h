// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameSetting.h"

#include "GameSettingValue.generated.h"

#define UE_API GAMESETTINGS_API

class UObject;

//--------------------------------------
// UGameSettingValue
//--------------------------------------

/**
 * The base class for all settings that are conceptually a value, that can be 
 * changed, and thus reset or restored to their initial value.
 *
 * 所有概念上为数值、可进行更改且能够重置或恢复至初始值的设置的基类。
 */
UCLASS(MinimalAPI, Abstract)
class UGameSettingValue : public UGameSetting
{
	GENERATED_BODY()

public:
	UE_API UGameSettingValue();

	/** Stores an initial value for the setting.  This will be called on initialize, but should also be called if you 'apply' the setting. */
	/** 用于存储设置的初始值。此方法会在初始化时调用，但在您“应用”该设置时也应调用此方法。*/
	UE_API virtual void StoreInitial() PURE_VIRTUAL(, );

	/** Resets the property to the default. */
	/** 将该属性重置为默认值。*/
	UE_API virtual void ResetToDefault() PURE_VIRTUAL(, );

	/** Restores the setting to the initial value, this is the value when you open the settings before making any tweaks. */
	/** 将设置恢复至初始值，即在进行任何调整之前打开设置时所使用的值。*/
	UE_API virtual void RestoreToInitial() PURE_VIRTUAL(, );

protected:
	// 确保描述文本不为空
	UE_API virtual void OnInitialized() override;
};

#undef UE_API
