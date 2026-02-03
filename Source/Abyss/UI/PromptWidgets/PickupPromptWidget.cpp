// Copyright (c) 2025 Leon Lee


#include "PickupPromptWidget.h"

#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Interfaces/InteractableInterface.h"

void UPickupPromptWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    ProgressBox->SetPercent(0.0f);
}

void UPickupPromptWidget::SetupInteraction(FInteractionData InInteractionData)
{
    MaxHoldTime = InInteractionData.InteractionDuration;
    if (PickupText)
    {
        PickupText->SetText(InInteractionData.ActionText);
    }

    // 重置所有状态
    CurrentHoldTime = 0.0f;
    bIsInteracting = false; // 这个变量现在代表 "Widget 是否在活动/更新中"
    bIsKeyHeld = false;
    
    ProgressBox->SetPercent(0.0f);
}

void UPickupPromptWidget::StartInteraction()
{
    // 如果是瞬时交互，直接触发，不需要进入 Tick 循环
    if (MaxHoldTime <= 0.0f)
    {
        OnShortPressTriggered.Broadcast();
        return;
    }

    bIsKeyHeld = true;
    bIsInteracting = true; // 开始 Tick
}

void UPickupPromptWidget::StopInteraction()
{
    // 玩家松手了
    bIsKeyHeld = false;
}

void UPickupPromptWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // 只有在交互逻辑激活时才计算
    if (bIsInteracting)
    {
        // 逻辑分支 A: 玩家按住按键 -> 增加时间
        if (bIsKeyHeld)
        {
            CurrentHoldTime += InDeltaTime;

            // 检查完成
            if (CurrentHoldTime >= MaxHoldTime)
            {
                OnLongPressComplete.Broadcast();
                
                // 完成后重置
                CurrentHoldTime = 0.0f;
                bIsInteracting = false;
                bIsKeyHeld = false;
            }
        }
        // 逻辑分支 B: 玩家松手了 -> 减少时间 (回退动画)
        else
        {
            // 回退速度可以快一点，比如 2 倍速，手感更好
            CurrentHoldTime -= (InDeltaTime * 2.0f);

            // 如果回退到 0，停止 Tick
            if (CurrentHoldTime <= 0.0f)
            {
                CurrentHoldTime = 0.0f;
                bIsInteracting = false; // 彻底停止
            }
        }

        // --- 更新进度条 ---
        float Alpha = (MaxHoldTime > 0.0f) ? FMath::Clamp(CurrentHoldTime / MaxHoldTime, 0.0f, 1.0f) : 0.0f;
        ProgressBox->SetPercent(Alpha);
    }
}
