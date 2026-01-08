// Copyright (c) 2025 Leon Lee


#include "AbyssBeamAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystem/AbilityTasks/TargetDataUnderCrosshair.h"
#include "AbilitySystem/BlueprintLibrary/AbyssAbilitySystemLibrary.h"
#include "Net/UnrealNetwork.h"


UAbyssBeamAbility::UAbyssBeamAbility()
{
    // 关键：开启 Ability 的复制，否则变量无法同步给其他客户端
    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UAbyssBeamAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    
    // 1. 播放 Start Montage
    if (BeamStartMontage)
    {
        // 创建 Task
        UAbilityTask_PlayMontageAndWait* StartMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this,
            NAME_None,       // Task 名字，通常给 NAME_None
            BeamStartMontage,
            1.0f,            // Rate
            NAME_None,       // StartSection
            false            // bStopWhenAbilityEnds
        );

        // 绑定回调：正常播放结束或混合出时，进入 Loop
        StartMontageTask->OnBlendOut.AddDynamic(this, &UAbyssBeamAbility::OnBeamStartMontageEnded);
        StartMontageTask->OnCompleted.AddDynamic(this, &UAbyssBeamAbility::OnBeamStartMontageEnded);

        // 绑定回调：异常中断时，取消技能
        StartMontageTask->OnInterrupted.AddDynamic(this, &UAbyssBeamAbility::OnBeamStartMontageFailed);
        StartMontageTask->OnCancelled.AddDynamic(this, &UAbyssBeamAbility::OnBeamStartMontageFailed);

        // 激活 Task
        StartMontageTask->ReadyForActivation();
    }

    // 2. 启动持续的 TargetData Task (每 DamageAndHitTickInterval 上报一次)
    UTargetDataUnderCrosshair* TargetTask = UTargetDataUnderCrosshair::CreateTargetDataUnderCrosshair(this, true, DamageAndHitTickInterval);
    TargetTask->ValidData.AddDynamic(this, &UAbyssBeamAbility::OnTargetDataReceived);
    TargetTask->ReadyForActivation();

    // 3. 监听松开按键 (WaitInputRelease)
    UAbilityTask_WaitInputRelease* WaitReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
    WaitReleaseTask->OnRelease.AddDynamic(this, &UAbyssBeamAbility::OnReleaseInput);
    WaitReleaseTask->ReadyForActivation();

    // 4. 添加持续的 GameplayCue 
    if (BeamCueTag.IsValid())
    {
        //TODO: 临时测试把AbilityLevel设置大了
        //CurrentCueParams.AbilityLevel = GetAbilityLevel();
        CurrentCueParams.AbilityLevel = 5;
        // 关键：在本地立刻添加 Cue，保证响应速度
        GetAbilitySystemComponentFromActorInfo()->AddGameplayCue(BeamCueTag, CurrentCueParams);
    }
}

void UAbyssBeamAbility::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle)
{
    AActor* HitActor = nullptr;
    if (DataHandle.Num() > 0)
    {
        const FHitResult* Hit = DataHandle.Get(0)->GetHitResult();
        if (Hit) HitActor = Hit->GetActor();
    }

    // 1. 本地预测 (Local Prediction)
    // 这种做法保证你自己玩的时候，光束是 0 延迟的
    if (CurrentHitTarget != HitActor)
    {
        CurrentHitTarget = HitActor;
    }

    // 2. 服务器权威逻辑 (Server Authority)
    if (HasAuthority(&CurrentActivationInfo))
    {
        // B. 造成伤害 (Server Only)
        if (HitActor)
        {
            CauseDamage(HitActor);
            ApplyChainDamage(HitActor);
        }
    }
}


void UAbyssBeamAbility::ApplyChainDamage(AActor* MainTarget)
{
    // 索敌逻辑
    TArray<AActor*> OverlappingActors;
    TArray<AActor*> ChainTargets;
    TArray<AActor*> IgnoreActors = { GetAvatarActorFromActorInfo(), MainTarget };

    UAbyssAbilitySystemLibrary::GetLiveCharacterWithinRadius(
        GetAvatarActorFromActorInfo(),
        OverlappingActors,
        IgnoreActors,
        ChainRadius,
        MainTarget->GetActorLocation()
    );

    UAbyssAbilitySystemLibrary::GetClosestTargets(
        GetAbilityLevel(),
        OverlappingActors,
        ChainTargets,
        MainTarget->GetActorLocation()
    );

    for (AActor* ChainTarget : ChainTargets)
    {
        CauseDamage(ChainTarget);
    }
}


void UAbyssBeamAbility::OnReleaseInput(float TimeHeld)
{
    // 移除 GameplayCue
    if (BeamCueTag.IsValid())
    {
        GetAbilitySystemComponentFromActorInfo()->RemoveGameplayCue(BeamCueTag);
    }

    // 播放 End Montage (收招动作)
    // 此时播放 EndMontage 会自动打断正在运行的 LoopMontage Task
    if (BeamEndMontage)
    {
        // 创建 End 动作的任务
        UAbilityTask_PlayMontageAndWait* EndTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this,
            NAME_None,
            BeamEndMontage,
            1.0f,
            NAME_None,       // StartSection
            false            // bStopWhenAbilityEnds: 设为 false，因为我们在 EndAbility 里会手动播放 EndMontage 来取代它
        );

        // 绑定回调
        EndTask->OnBlendOut.AddDynamic(this, &UAbyssBeamAbility::OnBeamEndMontageEnded);
        EndTask->OnInterrupted.AddDynamic(this, &UAbyssBeamAbility::OnBeamEndMontageEnded);
        EndTask->OnCancelled.AddDynamic(this, &UAbyssBeamAbility::OnBeamEndMontageEnded);
        EndTask->OnCompleted.AddDynamic(this, &UAbyssBeamAbility::OnBeamEndMontageEnded);
  
        // 激活任务
        EndTask->ReadyForActivation();
    }
    else
    {
        MontageStop();
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
    }
}

// 【新增】处理 Start -> Loop 的衔接
void UAbyssBeamAbility::OnBeamStartMontageEnded()
{
    // 安全检查：如果技能已经在前摇期间被取消了，就不要播 Loop 了
    if (!IsActive())
    {
        return;
    }
    if (BeamLoopMontage)
    {
        // 创建 Loop 动作的任务
        UAbilityTask_PlayMontageAndWait* LoopTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this,
            NAME_None,
            BeamLoopMontage,
            1.0f,
            NAME_None,       // StartSection
            false            // bStopWhenAbilityEnds: 设为 false，因为我们在 EndAbility 里会手动播放 EndMontage 来取代它
        );

        // 绑定回调
        // Loop 动作理论上不会自动 "Completed" (除非动画资源没勾选 Loop)，但如果被打断或混合出，我们需要知道
        LoopTask->OnBlendOut.AddDynamic(this, &UAbyssBeamAbility::OnBeamLoopMontageEnded);
        LoopTask->OnInterrupted.AddDynamic(this, &UAbyssBeamAbility::OnBeamLoopMontageEnded);
        LoopTask->OnCancelled.AddDynamic(this, &UAbyssBeamAbility::OnBeamLoopMontageEnded);
        // LoopTask->OnCompleted.AddDynamic(this, &UAbyssBeamAbility::OnBeamLoopMontageEnded); // 循环动作通常不绑定 Completed

        // 激活任务
        LoopTask->ReadyForActivation();
    }
}

// 处理 Start 被打断
void UAbyssBeamAbility::OnBeamStartMontageFailed()
{
    // 如果 Start 动作被打断（比如被击晕），通常意味着技能也该结束了
    K2_EndAbility();
}

// 处理 Loop 动作结束/中断
void UAbyssBeamAbility::OnBeamLoopMontageEnded()
{
    // 这里通常不需要做太多事情，因为：
    // 情况 A: 玩家松开按键 -> 触发 EndAbility -> 播放 EndMontage -> Loop 动作被新动作打断 -> 触发此回调
    // 情况 B: 玩家被晕眩 -> CancelAbility -> Loop 动作被打断 -> 触发此回调
    
    // 我们可以在这里做一个双重保险：
    // 如果动作停止了，但技能还在激活状态（且不是因为正在切换到 EndMontage），说明出问题了，强制结束技能
    // (需要排除正常结束的情况)
}

void UAbyssBeamAbility::OnBeamEndMontageEnded()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAbyssBeamAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}



















