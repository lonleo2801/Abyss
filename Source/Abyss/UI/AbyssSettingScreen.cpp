// Copyright (c) 2025 Leon Lee


#include "AbyssSettingScreen.h"

class UGameSettingRegistry;

void UAbyssSettingScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	/*BackHandle = RegisterUIActionBinding(FBindUIActionArgs(BackInputActionData,
		true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleBackAction)));
	ApplyHandle = RegisterUIActionBinding(FBindUIActionArgs(ApplyInputActionData,
		true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleApplyAction)));
	CancelChangesHandle = RegisterUIActionBinding(FBindUIActionArgs(CancelChangesInputActionData,
		true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleCancelChangesAction)));*/
}
/*UGameSettingRegistry* UAbyssSettingScreen::CreateRegistry()
{
	ULyraGameSettingRegistry* NewRegistry = NewObject<ULyraGameSettingRegistry>();

	if (ULyraLocalPlayer* LocalPlayer = CastChecked<ULyraLocalPlayer>(GetOwningLocalPlayer()))
	{
		// 极其重要
		NewRegistry->Initialize(LocalPlayer);
	}

	return NewRegistry;
	
}*/

void UAbyssSettingScreen::HandleBackAction()
{
	if (AttemptToPopNavigation())
	{
		return;
	}

	ApplyChanges();

	DeactivateWidget();
}

void UAbyssSettingScreen::HandleApplyAction()
{
	ApplyChanges();
}

void UAbyssSettingScreen::HandleCancelChangesAction()
{
	CancelChanges();
}


void UAbyssSettingScreen::OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty)
{
	// 如果有设置变动 则添加应用设置和取消设置的按钮
	if (bSettingsDirty)
	{
		if (!GetActionBindings().Contains(ApplyHandle))
		{
			AddActionBinding(ApplyHandle);
		}
		if (!GetActionBindings().Contains(CancelChangesHandle))
		{
			AddActionBinding(CancelChangesHandle);
		}
	}
	else
	{
		RemoveActionBinding(ApplyHandle);
		RemoveActionBinding(CancelChangesHandle);
	}
}