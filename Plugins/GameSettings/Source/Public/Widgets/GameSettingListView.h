// Copyright Epic Games, Inc. All Rights Reserved.
// Finished.
#pragma once

#include "Components/ListView.h"

#include "GameSettingListView.generated.h"

#define UE_API GAMESETTINGS_API

class STableViewBase;

class UGameSettingCollection;
class ULocalPlayer;
class UGameSettingVisualData;

/**
 * List of game settings.  Every entry widget needs to extend from GameSettingListEntryBase.
 * 游戏设置列表。每个条目控件都需要继承自 GameSettingListEntryBase 类。
 */
UCLASS(MinimalAPI, meta = (EntryClass = GameSettingListEntryBase))
class UGameSettingListView : public UListView
{
	GENERATED_BODY()

public:
	// 构造函数 无
	UE_API UGameSettingListView(const FObjectInitializer& ObjectInitializer);

	// 添加用于游戏设置的名称重写
	UE_API void AddNameOverride(const FName& DevName, const FText& OverrideName);

#if WITH_EDITOR
	// 编辑器接口 确保对应的控件资产必须要有!
	UE_API virtual void ValidateCompiledDefaults(IWidgetCompilerLog& InCompileLog) const override;
#endif

protected:
	UE_API virtual UUserWidget& OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable) override;

	UE_API virtual bool OnIsSelectableOrNavigableInternal(UObject* SelectedItem) override;

protected:
	// 指定的设置对应的控件类型数据!!!! 极其重要
	UPROPERTY(EditAnywhere)
	TObjectPtr<UGameSettingVisualData> VisualData;

private:
	TMap<FName, FText> NameOverrides;
};

#undef UE_API
