// Copyright (c) 2025 Leon Lee


#include "AbyssButtonBase.h"

#include "CommonActionWidget.h"


void UAbyssButtonBase::SetButtonText(const FText& InText)
{
	// 如果传递进来的是空值 那么就需要尝试读取输入控件的文本值用于更新 否则的话就使用传入进来的值进行更新
	bOverride_ButtonText = InText.IsEmpty();
	ButtonText = InText;
	
	RefreshButtonText();
}

void UAbyssButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();

	// 刷新风格
	UpdateButtonStyle();
	// 刷新文本
	RefreshButtonText();
	
}

void UAbyssButtonBase::UpdateInputActionWidget()
{
	Super::UpdateInputActionWidget();

	UpdateButtonStyle();
	
	RefreshButtonText();
}

void UAbyssButtonBase::OnInputMethodChanged(ECommonInputType CurrentInputType)
{
	Super::OnInputMethodChanged(CurrentInputType);

	UpdateButtonStyle();
}

void UAbyssButtonBase::RefreshButtonText()
{
	// 如果现在值是空的或者需要重写
	if (bOverride_ButtonText || ButtonText.IsEmpty())
	{
		// 读取输入控件的值
		if (InputActionWidget)
		{
			const FText ActionDisplayText = InputActionWidget->GetDisplayText();	
			if (!ActionDisplayText.IsEmpty())
			{
				UpdateButtonText(ActionDisplayText);
				return;
			}
		}
	}

	UpdateButtonText(ButtonText);	
	
}