// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "AbyssButtonBase.generated.h"

class UObject;
struct FFrame;


/**
 * UCommonButtonBase
 * Button that disables itself when not active. Also updates actions for CommonActionWidget if bound to display platform-specific icons.
 * 当不处于激活状态时会自动隐藏的按钮。此外，如果与显示平台特定图标相关联，还会为通用操作部件控件更新操作行为。
 */
/**
 * Lyra项目所使用的按钮基类
 * 该按钮在不处于激活状态时会自动隐藏。此外，如果与显示平台特定图标相关联，还会为通用操作部件框更新操作行为。
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class ABYSS_API UAbyssButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	void SetButtonText(const FText& InText);

protected:
	
	// UUserWidget interface
	virtual void NativePreConstruct() override;
	// End of UUserWidget interface
	
	// UCommonButtonBase interface
	/** 用于根据按钮的状态更新相关输入操作控件（如果有）的辅助函数 */
	virtual void UpdateInputActionWidget() override;

	
	/** 当输入法发生变化时通过委托机制调用此方法 */
	virtual void OnInputMethodChanged(ECommonInputType CurrentInputType) override;
	// End of UCommonButtonBase interface
	

	void RefreshButtonText();

	
	// 交给蓝图子类重写文本
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonText(const FText& InText);

	// 交给蓝图子类重写风格
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonStyle();

	
private:
	
	// 提供按钮重写文本
	UPROPERTY(EditAnywhere, Category="Button", meta=(InlineEditConditionToggle))
	uint8 bOverride_ButtonText : 1;

	// 重写所使用的文本 受到bOverride_ButtonText的限制
	UPROPERTY(EditAnywhere, Category="Button", meta=( editcondition="bOverride_ButtonText" ))
	FText ButtonText;

};
