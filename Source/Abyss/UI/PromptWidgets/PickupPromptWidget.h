// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "UI/AbyssActivatableWidget.h"
#include "PickupPromptWidget.generated.h"



// 定义交互结果的代理，方便外部（如玩家角色）监听
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLongPressComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShortPressTriggered);

struct FInteractionData;
class UProgressBar;
class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class ABYSS_API UPickupPromptWidget : public UAbyssActivatableWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// --- 外部调用接口 ---
    
	// 当玩家按下按键时调用
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void StartInteraction();

	// 当玩家松开按键时调用
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void StopInteraction();

	// 设置需要的长按时间（秒）
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetupInteraction(FInteractionData InInteractionData);

public:
	// --- UI 组件绑定 ---

	// 显示“拾取”文字
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PickupText;

	// 显示按键提示（如 "F"）
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> KeyPromptText;

	// 圆形进度条图片 (需要应用我们制作的材质)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBox;

public:
	// --- 事件 ---
    
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnLongPressComplete OnLongPressComplete;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnShortPressTriggered OnShortPressTriggered;

private:
	// 状态变量
	bool bIsInteracting = false; // 是否正在交互
	bool bIsKeyHeld = false; // 玩家是否按着键
	float CurrentHoldTime = 0.0f; // 当前长按时间
	float MaxHoldTime = 0.0f; // 最长长按时间
};
