// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Delegates/Delegate.h"

class ULocalPlayer;

//--------------------------------------
// FGameSettingDataSource
//--------------------------------------

class FGameSettingDataSource : public TSharedFromThis<FGameSettingDataSource>
{
public:
	virtual ~FGameSettingDataSource() { }

	/**
	 * Some settings may take an async amount of time to finish initializing.  The settings system will wait
	 * for all settings to be ready before showing the setting.
	 *
	 *  有些设置的初始化过程可能会需要相当长的时间。设置系统会一直等待所有设置都准备就绪后，才会显示这些设置
	 */
	virtual void Startup(ULocalPlayer* InLocalPlayer, FSimpleDelegate StartupCompleteCallback) { StartupCompleteCallback.ExecuteIfBound(); }

	// 解算是否成功
	virtual bool Resolve(ULocalPlayer* InContext) = 0;

	
	virtual FString GetValueAsString(ULocalPlayer* InContext) const = 0;

	virtual void SetValue(ULocalPlayer* InContext, const FString& Value) = 0;

	virtual FString ToString() const = 0;
};
