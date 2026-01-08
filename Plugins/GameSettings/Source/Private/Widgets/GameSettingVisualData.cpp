// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/GameSettingVisualData.h"

#include "GameSetting.h"
#include "Widgets/GameSettingListEntry.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameSettingVisualData)

TSubclassOf<UGameSettingListEntryBase> UGameSettingVisualData::GetEntryForSetting(UGameSetting* InSetting)
{
	if (InSetting == nullptr)
	{
		return TSubclassOf<UGameSettingListEntryBase>();
	}

	// Check if there's a custom logic for finding this setting's visual element
	// 检查是否有关于查找此设置的视觉元素的自定义逻辑
	TSubclassOf<UGameSettingListEntryBase> CustomEntry = GetCustomEntryForSetting(InSetting);
	if (CustomEntry)
	{
		return CustomEntry;
	}

	// Check if there's a specific entry widget for a setting by name.  Hopefully this is super rare.
	// 检查是否存在一个以名称命名的特定设置项组件。希望这种情况极为罕见。
	{
		TSubclassOf<UGameSettingListEntryBase> EntryWidgetClassPtr = EntryWidgetForName.FindRef(InSetting->GetDevName());
		if (EntryWidgetClassPtr)
		{
			return EntryWidgetClassPtr;
		}
	}

	// Finally check to see if there's an entry for this setting following the classes we have entries for.
	// we use the super chain of the setting classes to find the most applicable entry widget for this class
	// of setting.
	// 最后，检查一下是否在我们已有的设置项类别之后存在与此设置相关的条目。
	// 我们利用设置类的超类链来找到适用于此设置类的最合适的条目控件。
	for (UClass* Class = InSetting->GetClass(); Class; Class = Class->GetSuperClass())
	{
		if (TSubclassOf<UGameSetting> SettingClass = TSubclassOf<UGameSetting>(Class))
		{
			TSubclassOf<UGameSettingListEntryBase> EntryWidgetClassPtr = EntryWidgetForClass.FindRef(SettingClass);
			if (EntryWidgetClassPtr)
			{
				return EntryWidgetClassPtr;
			}
		}
	}

	return TSubclassOf<UGameSettingListEntryBase>();
}

TArray<TSoftClassPtr<UGameSettingDetailExtension>> UGameSettingVisualData::GatherDetailExtensions(UGameSetting* InSetting)
{
	TArray<TSoftClassPtr<UGameSettingDetailExtension>> Extensions;

	// Find extensions by setting name
	// 按设置名称查找扩展程序
	FGameSettingNameExtensions* ExtensionsWithName = ExtensionsForName.Find(InSetting->GetDevName());
	if (ExtensionsWithName)
	{
		Extensions.Append(ExtensionsWithName->Extensions);
		// 如果不需要包含默认的拓展就直接返回
		if (!ExtensionsWithName->bIncludeClassDefaultExtensions)
		{
			return Extensions;
		}
	}

	// Find extensions for it using the super chain of the setting so that we get any
	// class based extensions for this setting.
	// 利用设置的超链来查找其相关扩展，以便获取与该设置相关的任何基于类的扩展。
	for (UClass* Class = InSetting->GetClass(); Class; Class = Class->GetSuperClass())
	{
		if (TSubclassOf<UGameSetting> SettingClass = TSubclassOf<UGameSetting>(Class))
		{
			FGameSettingClassExtensions* ExtensionForClass = ExtensionsForClasses.Find(SettingClass);
			if (ExtensionForClass)
			{
				Extensions.Append(ExtensionForClass->Extensions);
			}
		}
	}

	return Extensions;
}

TSubclassOf<UGameSettingListEntryBase> UGameSettingVisualData::GetCustomEntryForSetting(UGameSetting* InSetting)
{
	return TSubclassOf<UGameSettingListEntryBase>();
}
