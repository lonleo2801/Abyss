// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameSetting.h"

#include "GameSettingCollection.generated.h"

#define UE_API GAMESETTINGS_API

struct FGameSettingFilterState;

//--------------------------------------
// UGameSettingCollection
//--------------------------------------

UCLASS(MinimalAPI)
class UGameSettingCollection : public UGameSetting
{
	GENERATED_BODY()

public:
	// 无
	UE_API UGameSettingCollection();

	// 获取子游戏设置
	virtual TArray<UGameSetting*> GetChildSettings() override { return Settings; }
	// 获取子游戏设置集合 如果有的话
	UE_API TArray<UGameSettingCollection*> GetChildCollections() const;

	// 添加游戏设置 同时初始化它
	UE_API void AddSetting(UGameSetting* Setting);
	// 通过过滤器获取游戏设置
	UE_API virtual void GetSettingsForFilter(const FGameSettingFilterState& FilterState, TArray<UGameSetting*>& InOutSettings) const;

	// 不可被选中
	virtual bool IsSelectable() const { return false; }

protected:
	/** The settings owned by this collection. */
	/** 本集合所拥有的设置。*/
	UPROPERTY(Transient)
	TArray<TObjectPtr<UGameSetting>> Settings;
};

//--------------------------------------
// UGameSettingCollectionPage
//--------------------------------------

UCLASS(MinimalAPI)
class UGameSettingCollectionPage : public UGameSettingCollection
{
	GENERATED_BODY()

public:
	// 执行导航事件的代理
	DECLARE_EVENT_OneParam(UGameSettingCollectionPage, FOnExecuteNavigation, UGameSetting* /*Setting*/);
	FOnExecuteNavigation OnExecuteNavigationEvent;

public:
	// 构造函数无
	UE_API UGameSettingCollectionPage();

	// 获取导航的文本
	FText GetNavigationText() const { return NavigationText; }
	// 设置导航的文本
	void SetNavigationText(FText Value) { NavigationText = Value; }
#if !UE_BUILD_SHIPPING
	// 测试时 设置导航的文本
	void SetNavigationText(const FString& Value) { SetNavigationText(FText::FromString(Value)); }
#endif
	// 非空判定
	UE_API virtual void OnInitialized() override;
	// 使用过滤器过滤 因为是嵌套页面
	UE_API virtual void GetSettingsForFilter(const FGameSettingFilterState& FilterState, TArray<UGameSetting*>& InOutSettings) const override;
	// 可以被选中点击
	virtual bool IsSelectable() const override { return true; }

	/** 执行导航 */
	UE_API void ExecuteNavigation();

private:
	// 导航的文本
	FText NavigationText;
};

#undef UE_API
