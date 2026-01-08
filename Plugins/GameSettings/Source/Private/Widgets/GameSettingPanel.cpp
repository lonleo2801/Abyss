// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/GameSettingPanel.h"

#include "CommonInputSubsystem.h"
#include "CommonInputTypeEnum.h"
#include "GameSettingRegistry.h"
#include "Widgets/GameSettingDetailView.h"
#include "Widgets/GameSettingListView.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameSettingPanel)

class SWidget;
struct FFocusEvent;
struct FGeometry;

UGameSettingPanel::UGameSettingPanel()
{
	SetIsFocusable(true);
}

void UGameSettingPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 在这里触发UMG悬停改变
	ListView_Settings->OnItemIsHoveredChanged().AddUObject(this, &ThisClass::HandleSettingItemHoveredChanged);
	ListView_Settings->OnItemSelectionChanged().AddUObject(this, &ThisClass::HandleSettingItemSelectionChanged);
}

void UGameSettingPanel::NativeConstruct()
{
	Super::NativeConstruct();

	UnregisterRegistryEvents();
	RegisterRegistryEvents();
}

void UGameSettingPanel::NativeDestruct()
{
	Super::NativeDestruct();

	UnregisterRegistryEvents();
}

FReply UGameSettingPanel::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	const UCommonInputSubsystem* InputSubsystem = GetInputSubsystem();
	if (InputSubsystem && InputSubsystem->GetCurrentInputType() == ECommonInputType::Gamepad)
	{
		if (TSharedPtr<SWidget> PrimarySlateWidget = ListView_Settings->GetCachedWidget())
		{
			ListView_Settings->NavigateToIndex(0);
			ListView_Settings->SetSelectedIndex(0);

			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}

void UGameSettingPanel::SetRegistry(UGameSettingRegistry* InRegistry)
{
	if (Registry != InRegistry)
	{
		UnregisterRegistryEvents();

		if (RefreshHandle.IsValid())
		{
			FTSTicker::GetCoreTicker().RemoveTicker(RefreshHandle);
		}

		Registry = InRegistry;

		RegisterRegistryEvents();

		RefreshSettingsList();
	}
}

void UGameSettingPanel::RegisterRegistryEvents()
{
	if (Registry)
	{
		Registry->OnSettingEditConditionChangedEvent.AddUObject(this, &ThisClass::HandleSettingEditConditionsChanged);
		Registry->OnSettingNamedActionEvent.AddUObject(this, &ThisClass::HandleSettingNamedAction);
		Registry->OnExecuteNavigationEvent.AddUObject(this, &ThisClass::HandleSettingNavigation);
	}
}

void UGameSettingPanel::UnregisterRegistryEvents()
{
	if (Registry)
	{
		Registry->OnSettingEditConditionChangedEvent.RemoveAll(this);
		Registry->OnSettingNamedActionEvent.RemoveAll(this);
		Registry->OnExecuteNavigationEvent.RemoveAll(this);
	}
}

void UGameSettingPanel::SetFilterState(const FGameSettingFilterState& InFilterState, bool bClearNavigationStack)
{
	FilterState = InFilterState;

	if (bClearNavigationStack)
	{
		FilterNavigationStack.Reset();
	}

	RefreshSettingsList();
}

bool UGameSettingPanel::CanPopNavigationStack() const
{
	return FilterNavigationStack.Num() > 0;
}

void UGameSettingPanel::PopNavigationStack()
{
	if (FilterNavigationStack.Num() > 0)
	{
		FilterState = FilterNavigationStack.Pop();
		RefreshSettingsList();
	}
}

void UGameSettingPanel::HandleSettingNavigation(UGameSetting* Setting)
{
	if (VisibleSettings.Contains(Setting))
	{
		FilterNavigationStack.Push(FilterState);

		// 新创建一个过滤的条件
		FGameSettingFilterState NewPageFilterState;
		NewPageFilterState.AddSettingToRootList(Setting);
		SetFilterState(NewPageFilterState, false);
	}
}

TArray<UGameSetting*> UGameSettingPanel::GetSettingsWeCanResetToDefault() const
{
	TArray<UGameSetting*> AvailableSettings;

	if (ensure(Registry->IsFinishedInitializing()))
	{
		// We want to get all available settings on this "screen" so we include the same allowlist, but ignore
		// 我们希望获取此“屏幕”上的所有可用设置，因此我们采用了相同的允许列表，但会忽略某些项。
		FGameSettingFilterState AllAvailableFilter = FilterState;
		AllAvailableFilter.bIncludeDisabled = true;
		AllAvailableFilter.bIncludeHidden = true;
		// 这里false是 判定屏蔽不可以恢复到默认的游戏设置选项.
		AllAvailableFilter.bIncludeResetable = false;
		AllAvailableFilter.bIncludeNestedPages = false;

		Registry->GetSettingsForFilter(AllAvailableFilter, AvailableSettings);
	}

	return AvailableSettings;
}

void UGameSettingPanel::RefreshSettingsList()
{
	// 如果当前有刷新的句柄 则不应当重入
	if (RefreshHandle.IsValid())
	{
		return;
	}

	RefreshHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateWeakLambda(this, [this](float DeltaTime)
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_UGameSettingPanel_RefreshSettingsList);

		if (Registry->IsFinishedInitializing())
		{
			VisibleSettings.Reset();
			Registry->GetSettingsForFilter(FilterState, MutableView(VisibleSettings));

			// 这里填充了我们需要显示的游戏设置项
			ListView_Settings->SetListItems(VisibleSettings);

			RefreshHandle.Reset();

			int32 IndexToSelect = 0;
			if (DesiredSelectionPostRefresh != NAME_None)
			{
				for (int32 SettingIdx = 0; SettingIdx < VisibleSettings.Num(); ++SettingIdx)
				{
					UGameSetting* Setting = VisibleSettings[SettingIdx];
					if (Setting->GetDevName() == DesiredSelectionPostRefresh)
					{
						IndexToSelect = SettingIdx;
						break;
					}
				}
				DesiredSelectionPostRefresh = NAME_None;
			}

			// If the list directly has the focus, instead of a child widget, then it's likely the panel and items
			// were not yet available when we received focus, so lets go ahead and focus the first item now.
			// 如果列表本身直接获得了焦点（而非某个子控件），那么很可能在我们获得焦点时，面板和其中的项目尚未准备好，所以现在就让我们先将第一个项目设为焦点吧。
			
			//if (HasUserFocus(GetOwningPlayer()))
			if (bAdjustListViewPostRefresh)
			{
				ListView_Settings->NavigateToIndex(IndexToSelect);
				ListView_Settings->SetSelectedIndex(IndexToSelect);
			}

			bAdjustListViewPostRefresh = true;

			// finally, refresh the editable state, but only once.
			// 最后，刷新可编辑状态，但仅执行一次。
			for (int32 SettingIdx = 0; SettingIdx < VisibleSettings.Num(); ++SettingIdx)
			{
				if (UGameSetting* Setting = VisibleSettings[SettingIdx])
				{
					Setting->RefreshEditableState(false);
				}
			}			

			return false;
		}

		return true;
	}));
}

void UGameSettingPanel::HandleSettingItemHoveredChanged(UObject* Item, bool bHovered)
{
	UGameSetting* Setting = bHovered ? Cast<UGameSetting>(Item) : ToRawPtr(LastHoveredOrSelectedSetting);
	if (bHovered && Setting)
	{
		LastHoveredOrSelectedSetting = Setting;
	}

	FillSettingDetails(Setting);
}

void UGameSettingPanel::HandleSettingItemSelectionChanged(UObject* Item)
{
	UGameSetting* Setting = Cast<UGameSetting>(Item);
	if (Setting)
	{
		LastHoveredOrSelectedSetting = Setting;
	}
	// 刷新细节面板 通过监听设置列表的变化随之变化!
	FillSettingDetails(Cast<UGameSetting>(Item));
}

void UGameSettingPanel::FillSettingDetails(UGameSetting* InSetting)
{
	if (Details_Settings)
	{
		Details_Settings->FillSettingDetails(InSetting);
	}

	OnFocusedSettingChanged.Broadcast(InSetting);
}

void UGameSettingPanel::HandleSettingNamedAction(UGameSetting* Setting, FGameplayTag GameSettings_Action_Tag)
{
	BP_OnExecuteNamedAction.Broadcast(Setting, GameSettings_Action_Tag);
}

void UGameSettingPanel::HandleSettingEditConditionsChanged(UGameSetting* Setting)
{
	const bool bWasSettingVisible = VisibleSettings.Contains(Setting);
	const bool bIsSettingVisible = Setting->GetEditState().IsVisible();

	if (bIsSettingVisible != bWasSettingVisible)
	{
		bAdjustListViewPostRefresh = Setting->GetAdjustListViewPostRefresh();
		RefreshSettingsList();
	}
}

void UGameSettingPanel::SelectSetting(const FName& SettingDevName)
{
	DesiredSelectionPostRefresh = SettingDevName;
	RefreshSettingsList();
}

UGameSetting* UGameSettingPanel::GetSelectedSetting() const
{
	return Cast<UGameSetting>(ListView_Settings->GetSelectedItem());
}

