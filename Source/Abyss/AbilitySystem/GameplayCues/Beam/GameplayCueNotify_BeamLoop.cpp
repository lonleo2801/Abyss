// Copyright (c) 2025 Leon Lee

#include "GameplayCueNotify_BeamLoop.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/BlueprintLibrary/AbyssAbilitySystemLibrary.h"


AGameplayCueNotify_BeamLoop::AGameplayCueNotify_BeamLoop()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bAutoDestroyOnRemove = true;
	// GameplayCueTag = AbyssTags_GameplayCue::Beam::Electrocute; // 建议在蓝图设置
}

bool AGameplayCueNotify_BeamLoop::WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (!MyTarget) return false;

	SourceActor = MyTarget; // 通常是 Pawn
	AbilityLevel = Parameters.AbilityLevel;

	// 1. 尝试缓存 Ability (为了 Pull Model)
	CacheSourceAbility();

	// 2. 生成瞬间的发射特效 (Muzzle Flash)
	if (BeamSpawnEffect)
	{
		BeamSpawnComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			BeamSpawnEffect,
			SourceActor->GetRootComponent(),
			BeamSocketName,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		);
	}

	// 3. 生成持续的主光束
	if (BeamLoopEffect)
	{
		BeamLoopComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			BeamLoopEffect,
			SourceActor->GetRootComponent(),
			BeamSocketName,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		);
	}

	// 4. 开启 Tick 进行持续更新
	SetActorTickEnabled(true);

	// 立即执行一次逻辑，防止第一帧画面延迟
	UpdateTargetFromAbility();
	UpdateMainBeamVisuals(0.0f); 

	return Super::WhileActive_Implementation(MyTarget, Parameters);
}

bool AGameplayCueNotify_BeamLoop::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	// 清理主光束
	if (BeamLoopComponent)
	{
		BeamLoopComponent->DestroyComponent();
		BeamLoopComponent = nullptr;
	}
	if (BeamSpawnComponent)
	{
		BeamSpawnComponent->DestroyComponent();
		BeamSpawnComponent = nullptr;
	}

	// 清理所有连锁光束
	CleanupAllBranches();

	SetActorTickEnabled(false);
	return Super::OnRemove_Implementation(MyTarget, Parameters);
}

// ============================================================================
// 核心 Tick 循环
// ============================================================================

void AGameplayCueNotify_BeamLoop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 1. 同步：从 C++ Ability 变量中拉取最新目标
	UpdateTargetFromAbility();

	// 2. 视觉：更新主光束 (带插值)
	UpdateMainBeamVisuals(DeltaTime);

	// 3. 逻辑：处理闪电链 (低频 0.1s)
	UpdateChainBeamLogic(DeltaTime);

	// 4. 视觉：更新闪电链 (每帧)
	UpdateChainBeamVisuals();
}

// ============================================================================
// 辅助功能实现
// ============================================================================

void AGameplayCueNotify_BeamLoop::CacheSourceAbility()
{
	if (!SourceActor) return;
	
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);
	if (!ASC) return;

	// 查找激活中的 UAbyssBeamAbility
	TArray<FGameplayAbilitySpec> Specs = ASC->GetActivatableAbilities();
	for (const FGameplayAbilitySpec& Spec : Specs)
	{
		if (Spec.Ability && Spec.Ability->IsA(UAbyssBeamAbility::StaticClass()) && Spec.IsActive())
		{
			SourceAbility = Cast<UAbyssBeamAbility>(Spec.GetPrimaryInstance());
			if (SourceAbility.IsValid()) break;
		}
	}
}

void AGameplayCueNotify_BeamLoop::UpdateTargetFromAbility()
{
	// 如果指针失效，重试缓存
	if (!SourceAbility.IsValid())
	{
		CacheSourceAbility();
	}

	// 从 Ability 读取 Replicated 变量 (Pull Model)
	if (SourceAbility.IsValid())
	{
		TargetActor = SourceAbility->CurrentHitTarget;
	}
}

void AGameplayCueNotify_BeamLoop::UpdateMainBeamVisuals(float DeltaTime)
{
	if (!BeamLoopComponent || !SourceActor) return;

	FVector DesiredEndPos;

	if (TargetActor)
	{
		// 有目标：指向目标中心 (或者可以用 Socket: Target->GetMesh()->GetSocketLocation("Spine_02"))
		DesiredEndPos = TargetActor->GetActorLocation();
	}
	else
	{
		// 无目标：指向正前方
		FVector StartPos;
		FRotator StartRot;
		SourceActor->GetActorEyesViewPoint(StartPos, StartRot); // 使用视线方向更准
		// 或者:
		// StartPos = SourceActor->GetActorLocation();
		// StartRot = SourceActor->GetActorForwardVector().Rotation();
		
		DesiredEndPos = StartPos + (StartRot.Vector() * DefaultTraceDistance);
	}

	// --- 视觉优化：插值平滑 ---
	// 获取当前特效设置的位置
	bool bHasValidLocation;
	FVector CurrentPos = BeamLoopComponent->GetVariableVec3(BeamEndName,bHasValidLocation); // Default设为Desired防止初始跳变
	
	// VInterpTo: Speed 越高越快。30.0f 感觉比较跟手，又不失平滑。
	FVector NewPos = FMath::VInterpTo(CurrentPos, DesiredEndPos, DeltaTime, 30.0f);

	BeamLoopComponent->SetVariableVec3(BeamEndName, NewPos);
}

void AGameplayCueNotify_BeamLoop::UpdateChainBeamLogic(float DeltaTime)
{
	ChainSearchTimer += DeltaTime;
	if (ChainSearchTimer < ChainSearchInterval) return; // 没到时间，跳过
	
	ChainSearchTimer = 0.0f;

	// 只有主目标存在且存活，才进行连锁判定
	// 注意：IsActorBeingDestroyed 检查很重要
	if (TargetActor && !TargetActor->IsActorBeingDestroyed())
	{
		FindAdditionalTargets();
		UpdateBeamBranches();
	}
	else
	{
		// 主目标没了，连锁必须断开
		CleanupAllBranches();
	}
}

void AGameplayCueNotify_BeamLoop::FindAdditionalTargets()
{
	// 使用你的库函数逻辑
	TArray<AActor*> OutOverlappingActors;
	const TArray<AActor*> ActorsToIgnore = { TargetActor, SourceActor };

	// 这里的 Origin 是 TargetActor 的位置 (从第一个敌人身上扩散)
	UAbyssAbilitySystemLibrary::GetLiveCharacterWithinRadius(
		SourceActor, 
		OutOverlappingActors, 
		ActorsToIgnore, 
		ChainRadius, 
		TargetActor->GetActorLocation()
	);

	AdditionalTargets.Reset();
	// 计算最近的目标
	UAbyssAbilitySystemLibrary::GetClosestTargets(
		AbilityLevel, 
		OutOverlappingActors, 
		AdditionalTargets, 
		TargetActor->GetActorLocation()
	);
}

void AGameplayCueNotify_BeamLoop::UpdateBeamBranches()
{
	// 1. 移除阶段 (Remove Old)
	for (int32 i = BeamBranches.Num() - 1; i >= 0; --i)
	{
		AActor* OldTarget = BeamBranches[i].Target.Get();
		
		// 如果目标不在新列表中，或者已经销毁
		if (!AdditionalTargets.Contains(OldTarget) || !OldTarget || OldTarget->IsActorBeingDestroyed())
		{
			if (BeamBranches[i].BeamComp)
			{
				BeamBranches[i].BeamComp->DestroyComponent();
			}
			BeamBranches.RemoveAt(i);
		}
	}

	// 2. 新增阶段 (Add New)
	if (!BeamLoopEffect) return; // 安全检查

	for (AActor* NewTarget : AdditionalTargets)
	{
		if (!NewTarget) continue;

		// 检查是否已经存在特效
		bool bExists = false;
		for (const FBeamBranchInfo& Info : BeamBranches)
		{
			if (Info.Target.Get() == NewTarget)
			{
				bExists = true;
				break;
			}
		}

		if (!bExists)
		{
			// 生成新的闪电链特效
			// 重要：Attach 到 TargetActor (主目标) 身上，因为是从主目标发出的
			UNiagaraComponent* NewComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
				BeamLoopEffect,
				TargetActor->GetRootComponent(),
				NAME_None,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget,
				true
			);

			// 初始位置设置，防止第一帧闪烁
			NewComp->SetVariableVec3(BeamEndName, NewTarget->GetActorLocation());

			FBeamBranchInfo NewBranch;
			NewBranch.BeamComp = NewComp;
			NewBranch.Target = NewTarget;
			BeamBranches.Add(NewBranch);
		}
	}
}

void AGameplayCueNotify_BeamLoop::UpdateChainBeamVisuals()
{
	// 只需要更新终点位置，起点是自动 Attach 在主目标身上的
	for (const FBeamBranchInfo& Branch : BeamBranches)
	{
		if (Branch.Target.IsValid() && Branch.BeamComp)
		{
			// 这里一般不需要插值，因为闪电链是吸附效果
			Branch.BeamComp->SetVariableVec3(BeamEndName, Branch.Target->GetActorLocation());
		}
	}
}

void AGameplayCueNotify_BeamLoop::CleanupAllBranches()
{
	if (BeamBranches.IsEmpty()) return;

	for (FBeamBranchInfo& Branch : BeamBranches)
	{
		if (Branch.BeamComp)
		{
			Branch.BeamComp->DestroyComponent();
		}
	}
	BeamBranches.Reset();
	AdditionalTargets.Reset();
}