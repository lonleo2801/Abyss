// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameSettingCollection.h"
#include "Templates/Casts.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameSettingCollection)

#define LOCTEXT_NAMESPACE "GameSetting"

//--------------------------------------
// UGameSettingCollection
//--------------------------------------

UGameSettingCollection::UGameSettingCollection()
{

}

void UGameSettingCollection::AddSetting(UGameSetting* Setting)
{
#if !UE_BUILD_SHIPPING
	ensureAlwaysMsgf(Setting->GetSettingParent() == nullptr, TEXT("This setting already has a parent!"));
	ensureAlwaysMsgf(!Settings.Contains(Setting), TEXT("This collection already includes this setting!"));
#endif

	Settings.Add(Setting);
	Setting->SetSettingParent(this);

	if (LocalPlayer)
	{
		Setting->Initialize(LocalPlayer);
	}
}

TArray<UGameSettingCollection*> UGameSettingCollection::GetChildCollections() const
{
	TArray<UGameSettingCollection*> CollectionSettings;

	for (UGameSetting* ChildSetting : Settings)
	{
		if (UGameSettingCollection* ChildCollection = Cast<UGameSettingCollection>(ChildSetting))
		{
			CollectionSettings.Add(ChildCollection);
		}
	}

	return CollectionSettings;
}

void UGameSettingCollection::GetSettingsForFilter(const FGameSettingFilterState& FilterState, TArray<UGameSetting*>& InOutSettings) const
{
	for (UGameSetting* ChildSetting : Settings)
	{
		// If the child setting is a collection, only add it to the set if it has any visible children.
		// 如果子项设置是一个集合，则仅在该集合中存在可见子项时才将其添加到集合中。
		if (Cast<UGameSettingCollectionPage>(ChildSetting))
		{
			if (FilterState.DoesSettingPassFilter(*ChildSetting))
			{
				InOutSettings.Add(ChildSetting);
			}
		}
		else if (UGameSettingCollection* ChildCollection = Cast<UGameSettingCollection>(ChildSetting))
		{
			TArray<UGameSetting*> ChildSettings;
			ChildCollection->GetSettingsForFilter(FilterState, ChildSettings);

			if (ChildSettings.Num() > 0)
			{
				// Don't add the root setting to the returned items, it's the container of N-possible 
				// other settings and containers we're actually displaying right now.
				// 不要在返回的项目中添加根设置，因为该设置是包含 N 种其他设置及容器的集合，而我们此刻实际展示的正是这些设置及容器的集合。
				if (!FilterState.IsSettingInRootList(ChildSetting))
				{
					InOutSettings.Add(ChildSetting);
				}

				InOutSettings.Append(ChildSettings);
			}
		}
		else
		{
			//  如果只是游戏设置 直接过滤器判定即可
			if (FilterState.DoesSettingPassFilter(*ChildSetting))
			{
				InOutSettings.Add(ChildSetting);
			}
		}
	}
}

//--------------------------------------
// UGameSettingCollectionPage
//--------------------------------------

UGameSettingCollectionPage::UGameSettingCollectionPage()
{
}

void UGameSettingCollectionPage::OnInitialized()
{
	Super::OnInitialized();

#if !UE_BUILD_SHIPPING
	ensureAlwaysMsgf(!NavigationText.IsEmpty(), TEXT("You must provide a NavigationText for this setting."));
	ensureAlwaysMsgf(!DescriptionRichText.IsEmpty(), TEXT("You must provide a description for new page collections."));
#endif
}

void UGameSettingCollectionPage::GetSettingsForFilter(const FGameSettingFilterState& FilterState, TArray<UGameSetting*>& InOutSettings) const
{
	// If we're including nested pages, call the super and dump them all, otherwise, we pretend we have none for the filtering.
	// because our settings are displayed on another page.
	// 如果包含嵌套页面，则调用父类方法并一次性将所有页面都列出，否则，我们就假装没有这些页面用于筛选操作。
	// 因为我们的设置是在另一个页面中显示的。
	if (FilterState.bIncludeNestedPages || FilterState.IsSettingInRootList(this))
	{
		Super::GetSettingsForFilter(FilterState, InOutSettings);
	}
}

void UGameSettingCollectionPage::ExecuteNavigation()
{
	OnExecuteNavigationEvent.Broadcast(this);
}

#undef LOCTEXT_NAMESPACE

