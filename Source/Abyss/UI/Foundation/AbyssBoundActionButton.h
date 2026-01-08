// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Input/CommonBoundActionButton.h"
#include "AbyssBoundActionButton.generated.h"

class UCommonButtonStyle;
class UObject;

/**
 * 底部按钮
 * 用以切换按钮的样式,在输入方法改变时.
 */
UCLASS(Abstract, meta = (DisableNativeTick))
class ABYSS_API UAbyssBoundActionButton : public UCommonBoundActionButton
{
	GENERATED_BODY()

protected:
	
	virtual void NativeConstruct() override;

private:

	void HandleInputMethodChanged(ECommonInputType NewInputMethod);
	
	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> KeyboardStyle;

	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> GamepadStyle;

	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> TouchStyle;
	
};
