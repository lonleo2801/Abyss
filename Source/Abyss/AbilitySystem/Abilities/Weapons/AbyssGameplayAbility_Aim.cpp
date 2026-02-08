// Copyright (c) 2025 Leon Lee


#include "AbyssGameplayAbility_Aim.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"


UAbyssGameplayAbility_Aim::UAbyssGameplayAbility_Aim()
{
    // 瞄准必须是本地预测的，这样手感才没有延迟
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    
    // 必须是实例化 (Instanced)，因为我们需要保存 AimingEffectHandle 和运行 Task
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    // (可选) 设置只能在角色存活时瞄准
    // ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead"));
}

void UAbyssGameplayAbility_Aim::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 1. 让SandboxCharacter_Mover进入want to Aiming state
    BP_PawnEnterWantToAimState();

    // 2. 启动 "等待松手" 任务 (WaitInputRelease)
    // 这是处理 "长按" 逻辑的标准 GAS 做法
    UAbilityTask_WaitInputRelease* WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
    if (WaitInputReleaseTask)
    {
        // 绑定回调函数
        WaitInputReleaseTask->OnRelease.AddDynamic(this, &UAbyssGameplayAbility_Aim::OnInputReleased);
        // 激活任务
        WaitInputReleaseTask->ReadyForActivation();
    }
    else
    {
        // 如果任务创建失败，直接结束
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    }
}

void UAbyssGameplayAbility_Aim::OnInputReleased(float TimeHeld)
{
    // 玩家松手了 -> 正常结束 Ability
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAbyssGameplayAbility_Aim::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    // 让SandboxCharacter_Mover 退出 want to Aiming state
    BP_PawnExitWantToAimState();

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}