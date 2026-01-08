// Copyright (c) 2025 Leon Lee


#include "AbyssBoundActionButton.h"

#include "CommonInputSubsystem.h"


class UCommonButtonStyle;

void UAbyssBoundActionButton::NativeConstruct()
{
	Super::NativeConstruct();

	
	if (UCommonInputSubsystem* InputSubsystem = GetInputSubsystem())
	{
		InputSubsystem->OnInputMethodChangedNative.AddUObject(this, &ThisClass::HandleInputMethodChanged);
		HandleInputMethodChanged(InputSubsystem->GetCurrentInputType());
	}
}

void UAbyssBoundActionButton::HandleInputMethodChanged(ECommonInputType NewInputMethod)
{
	TSubclassOf<UCommonButtonStyle> NewStyle = nullptr;

	if (NewInputMethod == ECommonInputType::Gamepad)
	{
		NewStyle = GamepadStyle;
	}
	else if (NewInputMethod == ECommonInputType::Touch)
	{
		NewStyle = TouchStyle;
	}
	else
	{
		NewStyle = KeyboardStyle;
	}


	if (NewStyle)
	{
		SetStyle(NewStyle);
	}
}