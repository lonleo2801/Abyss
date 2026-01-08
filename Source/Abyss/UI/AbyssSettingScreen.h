// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Widgets/GameSettingScreen.h"
#include "AbyssSettingScreen.generated.h"

class UAbyssTabListWidgetBase;
// 设置的最上层面板
// 1.响应输入 如回退,确认,取消
// 2.如果有变动就显示确认和取消的按钮
// 3.创建游戏设置注册器UGameSettingRegistry!!!极其重要.是我们游戏设置有多少选项的底层入口!
// 4.持有顶部标签栏,当我们点击或者注册Tab页面时,更新具体设置面板
// 5.持有底部侧边栏,用以显示底部按钮
UCLASS(Abstract, meta = (Category = "Settings", DisableNativeTick))
class ABYSS_API UAbyssSettingScreen : public UGameSettingScreen
{
	GENERATED_BODY()
	
public:

protected:

	// 绑定输入映射
	virtual void NativeOnInitialized() override;

	// 极其重要!!! 创建设置注册器 调用逻辑见父类
	// 这里会去初始化我们底层有多少游戏设置.
	//virtual UGameSettingRegistry* CreateRegistry() override;

	// 处理返回 尝试回退到上一次导航的位置(嵌套页面) 如果没有的话就应用变化 并退出
	void HandleBackAction();
	// 应用设置
	void HandleApplyAction();
	// 取消设置应用
	void HandleCancelChangesAction();

	// 设置变动时显示 应用和取消按钮
	virtual void OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty) override;


protected:
	// 绑定控件 用于切换显示的分类设置
	/*UPROPERTY(BlueprintReadOnly, Category = Input, meta = (BindWidget, OptionalWidget = true, AllowPrivateAccess = true))
	TObjectPtr<UAbyssTabListWidgetBase> TopSettingsTabs;*/

	// 返回的输入映射
	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle BackInputActionData;

	// 应用的输入映射
	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle ApplyInputActionData;

	// 取消的输入映射
	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle CancelChangesInputActionData;

	// 返回事件的句柄
	FUIActionBindingHandle BackHandle;

	// 应用设置的句柄
	FUIActionBindingHandle ApplyHandle;

	// 取消改变的句柄
	FUIActionBindingHandle CancelChangesHandle;

};
