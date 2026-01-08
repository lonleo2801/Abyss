// Copyright (c) 2025 Leon Lee


#include "AbyssActivatableWidget.h"

#include "Editor/WidgetCompilerLog.h"

UAbyssActivatableWidget::UAbyssActivatableWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TOptional<FUIInputConfig> UAbyssActivatableWidget::GetDesiredInputConfig() const
{
	switch (InputConfig)
	{
	case ELyraWidgetInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
	case ELyraWidgetInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
	case ELyraWidgetInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
	case ELyraWidgetInputMode::Default:
	default:
		return TOptional<FUIInputConfig>();
	}

	
}


#if WITH_EDITOR
void UAbyssActivatableWidget::ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree,
	class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledWidgetTree(BlueprintWidgetTree, CompileLog);

	if (!GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UAbyssActivatableWidget, BP_GetDesiredFocusTarget)))
	{
		if (GetParentNativeClass(GetClass()) == UAbyssActivatableWidget::StaticClass())
		{
			//CompileLog.Warning(LOCTEXT("ValidateGetDesiredFocusTarget_Warning", "GetDesiredFocusTarget wasn't implemented, you're going to have trouble using gamepads on this screen."));
		}
		else
		{
			//TODO - Note for now, because we can't guarantee it isn't implemented in a native subclass of this one.
			// 临时备注 - 目前暂不做确认，因为无法保证它不会在本类的原生子类中实现。
			//CompileLog.Note(LOCTEXT("ValidateGetDesiredFocusTarget_Note", "GetDesiredFocusTarget wasn't implemented, you're going to have trouble using gamepads on this screen.  If it was implemented in the native base class you can ignore this message."));
		}
	}
}
#endif
