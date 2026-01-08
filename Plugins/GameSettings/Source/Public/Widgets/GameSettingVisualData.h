// Copyright Epic Games, Inc. All Rights Reserved.
// Finished.
#pragma once

#include "Engine/DataAsset.h"

#include "GameSettingVisualData.generated.h"

#define UE_API GAMESETTINGS_API

class FName;
class UGameSetting;
class UGameSettingDetailExtension;
class UGameSettingListEntryBase;
class UObject;

// 以游戏设置的类进行拓展
USTRUCT(BlueprintType)
struct FGameSettingClassExtensions
{
	GENERATED_BODY()

public:
	// 游戏设置对应的拓展控件
	UPROPERTY(EditAnywhere, Category = Extensions)
	TArray<TSoftClassPtr<UGameSettingDetailExtension>> Extensions;
};
// 以游戏设置的名称进行拓展
USTRUCT(BlueprintType)
struct FGameSettingNameExtensions
{
	GENERATED_BODY()

public:
	// 是否包含默认的拓展
	UPROPERTY(EditAnywhere, Category = Extensions)
	bool bIncludeClassDefaultExtensions = false;

	// 游戏设置对应的拓展控件
	UPROPERTY(EditAnywhere, Category = Extensions)
	TArray<TSoftClassPtr<UGameSettingDetailExtension>> Extensions;
};

/**
 * 用来映射不同游戏设置选项对应的实例化控件类型
 * 极其重要!!!
 */
UCLASS(MinimalAPI, BlueprintType)
class UGameSettingVisualData : public UDataAsset
{
	GENERATED_BODY()

public:
	UE_API TSubclassOf<UGameSettingListEntryBase> GetEntryForSetting(UGameSetting* InSetting);

	UE_API virtual TArray<TSoftClassPtr<UGameSettingDetailExtension>> GatherDetailExtensions(UGameSetting* InSetting);
	
protected:
	// 无
	UE_API virtual TSubclassOf<UGameSettingListEntryBase> GetCustomEntryForSetting(UGameSetting* InSetting);

protected:
	// 针对某一类型的游戏设置使用该控件类型进行实例化 一般用这个
	UPROPERTY(EditDefaultsOnly, Category = ListEntries, meta = (AllowAbstract))
	TMap<TSubclassOf<UGameSetting>, TSubclassOf<UGameSettingListEntryBase>> EntryWidgetForClass;
	// 针对某一个游戏设置使用该控件类型进行实例化 一般用不到
	UPROPERTY(EditDefaultsOnly, Category = ListEntries, meta = (AllowAbstract))
	TMap<FName, TSubclassOf<UGameSettingListEntryBase>> EntryWidgetForName;

	UPROPERTY(EditDefaultsOnly, Category = Extensions, meta = (AllowAbstract))
	TMap<TSubclassOf<UGameSetting>, FGameSettingClassExtensions> ExtensionsForClasses;

	UPROPERTY(EditDefaultsOnly, Category = Extensions)
	TMap<FName, FGameSettingNameExtensions> ExtensionsForName;
};

#undef UE_API
