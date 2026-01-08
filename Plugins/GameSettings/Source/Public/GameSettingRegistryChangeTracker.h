// Copyright Epic Games, Inc. All Rights Reserved.
// Finshed.
#pragma once

#include "UObject/ObjectKey.h"
#include "UObject/WeakObjectPtrTemplates.h"

#define UE_API GAMESETTINGS_API

enum class EGameSettingChangeReason : uint8;

class UGameSetting;
class UGameSettingRegistry;
struct FObjectKey;

/**
 * FNoncopyable
 * 用于不能被复制的类的实用模板。
 * 按照此类进行派生，即可使您的类无法被复制
 */
class FGameSettingRegistryChangeTracker : public FNoncopyable
{
public:
	UE_API FGameSettingRegistryChangeTracker();
	UE_API ~FGameSettingRegistryChangeTracker();

	// 清除之前的绑定 并重新绑定的新的游戏设置注册器
	UE_API void WatchRegistry(UGameSettingRegistry* InRegistry);
	// 清除之前的绑定 
	UE_API void StopWatchingRegistry();

	// 应用改变过的游戏设置变化
	UE_API void ApplyChanges();

	// 恢复改变过的游戏设置为初始值
	UE_API void RestoreToInitial();

	// 清除修改过的记录
	UE_API void ClearDirtyState();

	// 是否正在复原设置 避免同一时间 多次修改
	bool IsRestoringSettings() const { return bRestoringSettings; }

	// 是否有设置被修改过
	bool HaveSettingsBeenChanged() const { return bSettingsChanged; }

private:
	// 监听设置变化的事件 绑定在游戏设置注册器上
	UE_API void HandleSettingChanged(UGameSetting* Setting, EGameSettingChangeReason Reason);

	// 是否有设置变动过
	bool bSettingsChanged = false;
	// 是否正在复原设置
	bool bRestoringSettings = false;

	// 监听的游戏设置注册器
	TWeakObjectPtr<UGameSettingRegistry> Registry;

	// 被修改过的设置
	TMap<FObjectKey, TWeakObjectPtr<UGameSetting>> DirtySettings;
};

#undef UE_API
