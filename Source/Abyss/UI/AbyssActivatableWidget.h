// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "AbyssActivatableWidget.generated.h"


UENUM(BlueprintType)
enum class ELyraWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ABYSS_API UAbyssActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

	
public:
	
	UAbyssActivatableWidget(const FObjectInitializer& ObjectInitializer);
	
	//~UCommonActivatableWidget interface
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	//~End of UCommonActivatableWidget interface

#if WITH_EDITOR

	/**
	* 在编译结束时，可覆盖默认的组件树执行任何自定义检查。*
	* 注意：此时此用户组件的 WidgetTree 和 BindWidget 属性尚未建立，
	* 因此请仅检查所提供的 BlueprintWidgetTree。*
	* 提示：若需验证 BindWidget 成员的属性值，您可以在组件树中依据属性名称进行搜索。*/
	virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, class IWidgetCompilerLog& CompileLog) const override;
#endif
	
	
protected:
	/** The desired input mode to use while this UI is activated, for example do you want key presses to still reach the game/player controller? */
	/** 当此用户界面被激活时所期望使用的输入模式，例如您是否希望键盘按键仍能传递给游戏/玩家控制器？*/
	UPROPERTY(EditDefaultsOnly, Category = Input)
	ELyraWidgetInputMode InputConfig = ELyraWidgetInputMode::Default;

	/** The desired mouse behavior when the game gets input. */
	/** 游戏接收到输入时所期望的鼠标行为。*/
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
};
