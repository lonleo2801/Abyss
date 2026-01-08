// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/GameSettingListView.h"
#include "Widgets/GameSettingListEntry.h"
#include "Widgets/GameSettingVisualData.h"


#include "GameSettingCollection.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameSettingListView)

#if WITH_EDITOR
#include "Editor/WidgetCompilerLog.h"
#endif

#define LOCTEXT_NAMESPACE "GameSetting"

UGameSettingListView::UGameSettingListView(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if WITH_EDITOR

void UGameSettingListView::ValidateCompiledDefaults(IWidgetCompilerLog& InCompileLog) const
{
	Super::ValidateCompiledDefaults(InCompileLog);

	if (!VisualData)
	{
		InCompileLog.Error(FText::Format(FText::FromString("{0} has no VisualData defined."), FText::FromString(GetName())));
	}
}

#endif

UUserWidget& UGameSettingListView::OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable)
{
	// 转换成游戏设置项
	UGameSetting* SettingItem = Cast<UGameSetting>(Item);

	// 默认的实例化控件类
	TSubclassOf<UGameSettingListEntryBase> SettingEntryClass = TSubclassOf<UGameSettingListEntryBase>(DesiredEntryClass);

	// 需要走查询控件对应的资产
	if (VisualData)
	{
		if (const TSubclassOf<UGameSettingListEntryBase> EntryClassSetting = VisualData->GetEntryForSetting(SettingItem))
		{
			// 覆写!
			SettingEntryClass = EntryClassSetting;
		}
		else
		{
			//UE_LOG(LogGameSettings, Error, TEXT("UGameSettingListView: No Entry Class Found!"));
		}
	}
	else
	{
		//UE_LOG(LogGameSettings, Error, TEXT("UGameSettingListView: No VisualData Defined!"));
	}

	// 生成控件
	UGameSettingListEntryBase& EntryWidget = GenerateTypedEntry<UGameSettingListEntryBase>(SettingEntryClass, OwnerTable);

	// 用于重写一下显示的名称
	if (!IsDesignTime())
	{
		if (const FText* Override = NameOverrides.Find(SettingItem->GetDevName()))
		{
			EntryWidget.SetDisplayNameOverride(*Override);
		}

		EntryWidget.SetSetting(SettingItem);
	}

	return EntryWidget;
}

bool UGameSettingListView::OnIsSelectableOrNavigableInternal(UObject* SelectedItem)
{
	if (const UGameSettingCollection* CollectionItem = Cast<UGameSettingCollection>(SelectedItem))
	{
		return CollectionItem->IsSelectable();
	}

	return true;
}

void UGameSettingListView::AddNameOverride(const FName& DevName, const FText& OverrideName)
{
	NameOverrides.Add(DevName, OverrideName);
}

#undef LOCTEXT_NAMESPACE
