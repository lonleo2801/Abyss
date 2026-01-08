// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameSettingFilterState.h"
#include "GameplayTagContainer.h"

#define UE_API GAMESETTINGS_API

class ULocalPlayer;

//////////////////////////////////////////////////////////////////////
// FWhenPlatformHasTrait

// Edit condition for game settings that checks CommonUI's platform traits
// to determine whether or not to show a setting
// 对游戏设置的编辑条件，该条件会检查 CommonUI 的平台特性，以决定是否显示某个设置选项
class FWhenPlatformHasTrait : public FGameSettingEditCondition
{
public:
	// 创建一个平台特征查询 如果缺失就移除该项
	static UE_API TSharedRef<FWhenPlatformHasTrait> KillIfMissing(FGameplayTag InVisibilityTag, const FString& InKillReason);
	// 创建一个平台特征查询 如果缺失就禁用该项
	static UE_API TSharedRef<FWhenPlatformHasTrait> DisableIfMissing(FGameplayTag InVisibilityTag, const FText& InDisableReason);
	
	// 创建一个平台特征查询 如果拥有就移除该项
	static UE_API TSharedRef<FWhenPlatformHasTrait> KillIfPresent(FGameplayTag InVisibilityTag, const FString& InKillReason);
	// 创建一个平台特征查询 如果拥有就禁用该项
	static UE_API TSharedRef<FWhenPlatformHasTrait> DisableIfPresent(FGameplayTag InVisibilityTag, const FText& InDisableReason);

	// 计算编辑结果
	//~FGameSettingEditCondition interface
	UE_API virtual void GatherEditState(const ULocalPlayer* InLocalPlayer, FGameSettingEditableState& InOutEditState) const override;
	//~End of FGameSettingEditCondition interface

private:

	// 对应的Tag
	FGameplayTag VisibilityTag;

	// 是否需求该Tag
	bool bTagDesired;
	
	// 移除原因
	FString KillReason;

	// 禁用原因
	FText DisableReason;
};

#undef UE_API
