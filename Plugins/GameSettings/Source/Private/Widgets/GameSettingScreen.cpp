// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/GameSettingScreen.h"

#include "GameSettingCollection.h"
#include "Widgets/GameSettingPanel.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameSettingScreen)

class UWidget;

#define LOCTEXT_NAMESPACE "GameSetting"

void UGameSettingScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UGameSettingScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	ChangeTracker.WatchRegistry(Registry);
	// 初始化一下 当前应该无设置变动
	OnSettingsDirtyStateChanged(HaveSettingsBeenChanged());
}

void UGameSettingScreen::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
}

UGameSettingRegistry* UGameSettingScreen::GetOrCreateRegistry()
{
	if (Registry == nullptr)
	{
		// 这里是交由子类去实现创建游戏设置注册器GameSettingRegistry
		UGameSettingRegistry* NewRegistry = this->CreateRegistry();
		// 有设置变化的代理回调绑定
		NewRegistry->OnSettingChangedEvent.AddUObject(this, &ThisClass::HandleSettingChanged);

		// 让显示具体设置的面板匹配游戏设置注册器
		Settings_Panel->SetRegistry(NewRegistry);

		// 持有游戏设置注册器 避免GC
		Registry = NewRegistry;
	}

	return Registry;
}

UWidget* UGameSettingScreen::NativeGetDesiredFocusTarget() const
{
	if (UWidget* Target = BP_GetDesiredFocusTarget())
	{
		return Target;
	}

	return Settings_Panel;
}

void UGameSettingScreen::ApplyChanges()
{
	if (ChangeTracker.HaveSettingsBeenChanged())
	{
		ChangeTracker.ApplyChanges();
		ClearDirtyState();
		Registry->SaveChanges();
	}
}

void UGameSettingScreen::CancelChanges()
{
	ChangeTracker.RestoreToInitial();
	ClearDirtyState();
}

void UGameSettingScreen::ClearDirtyState()
{
	ChangeTracker.ClearDirtyState();

	OnSettingsDirtyStateChanged(false);
}

bool UGameSettingScreen::AttemptToPopNavigation()
{
	if (Settings_Panel->CanPopNavigationStack())
	{
		Settings_Panel->PopNavigationStack();
		return true;
	}

	return false;
}

UGameSettingCollection* UGameSettingScreen::GetSettingCollection(FName SettingDevName, bool& HasAnySettings)
{
	HasAnySettings = false;
	
	if (UGameSettingCollection* Collection = GetRegistry()->FindSettingByDevNameChecked<UGameSettingCollection>(SettingDevName))
	{
		TArray<UGameSetting*> InOutSettings;
		
		FGameSettingFilterState FilterState;
		Collection->GetSettingsForFilter(FilterState, InOutSettings);
		// 检测 是否有游戏设置 如果是空项目 调用者应当知道是否可以创建空的面板按钮!
		HasAnySettings = InOutSettings.Num() > 0;
		
		return Collection;
	}

	return nullptr;
}

void UGameSettingScreen::NavigateToSetting(FName SettingDevName)
{
	NavigateToSettings({SettingDevName});
}

void UGameSettingScreen::NavigateToSettings(const TArray<FName>& SettingDevNames)
{
	FGameSettingFilterState FilterState;

	// 创建好一个新的过滤器
	for (const FName SettingDevName : SettingDevNames)
	{
		// 在游戏设置注册器里面寻找是否有该游戏设置
		// 注意这里一般找的是UGameSettingCollection  它是GameSetting的子类
		if (UGameSetting* Setting = GetRegistry()->FindSettingByDevNameChecked<UGameSetting>(SettingDevName))
		{
			FilterState.AddSettingToRootList(Setting);
		}
	}

	// 使用该过滤器去过滤设置具体面板
	Settings_Panel->SetFilterState(FilterState);
}

void UGameSettingScreen::HandleSettingChanged(UGameSetting* Setting, EGameSettingChangeReason Reason)
{
	OnSettingsDirtyStateChanged(true);
}

#undef LOCTEXT_NAMESPACE
