// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameSetting.h"

#include "GameSettingAction.generated.h"

#define UE_API GAMESETTINGS_API

//--------------------------------------
// UGameSettingAction
//--------------------------------------

class ULocalPlayer;

DECLARE_DELEGATE_TwoParams(UGameSettingCustomAction, UGameSetting* /*Setting*/, ULocalPlayer* /*LocalPlayer*/)

/**
 * 
 */
UCLASS(MinimalAPI)
class UGameSettingAction : public UGameSetting
{
	GENERATED_BODY()

public:
	UE_API UGameSettingAction();

public:

	DECLARE_EVENT_TwoParams(UGameSettingAction, FOnExecuteNamedAction, UGameSetting* /*Setting*/, FGameplayTag /*GameSettings_Action_Tag*/);
	FOnExecuteNamedAction OnExecuteNamedActionEvent;

public:

	FText GetActionText() const { return ActionText; }
	void SetActionText(FText Value) { ActionText = Value; }
#if !UE_BUILD_SHIPPING
	void SetActionText(const FString& Value) { SetActionText(FText::FromString(Value)); }
#endif

	FGameplayTag GetNamedAction() const { return NamedAction; }
	void SetNamedAction(FGameplayTag Value) { NamedAction = Value; }

	bool HasCustomAction() const { return CustomAction.IsBound(); }
	void SetCustomAction(UGameSettingCustomAction InAction) { CustomAction = InAction; }
	UE_API void SetCustomAction(TFunction<void(ULocalPlayer*)> InAction);

	/**
	 * By default actions don't dirty the settings, since the majority of them either do things you can't
	 * revert, or they're things like show the credits or Eula.  However if that's not true, set this flag
	 * to true to fire the change event when this action is fired.
	 */
	/**
	 * 	默认情况下，操作不会影响设置，因为大多数操作要么无法恢复，要么就是诸如显示片尾字幕或许可协议这类内容。然而，如果情况并非如此，可将此标志设置为“真”，以便在执行此操作时触发更改事件。
	 */
	void SetDoesActionDirtySettings(bool Value) { bDirtyAction = Value; }

	UE_API virtual void ExecuteAction();

protected:
	/** UGameSettingValue */
	UE_API virtual void OnInitialized() override;

protected:
	FText ActionText;
	FGameplayTag NamedAction;
	UGameSettingCustomAction CustomAction;
	bool bDirtyAction = false;
};

#undef UE_API
