// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "NiagaraSystem.h"
#include "AbilitySystem/Abilities/Beam/AbyssBeamAbility.h" // 确保护径正确
#include "GameplayCueNotify_BeamLoop.generated.h"

class UNiagaraComponent;

// 每条链路 Beam 的追踪数据
USTRUCT()
struct FBeamBranchInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> BeamComp = nullptr;

	UPROPERTY()
	TWeakObjectPtr<AActor> Target = nullptr;
};

UCLASS()
class ABYSS_API AGameplayCueNotify_BeamLoop : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	AGameplayCueNotify_BeamLoop();

	// GC 生命周期
	virtual bool WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	
	// 核心循环
	virtual void Tick(float DeltaTime) override;

protected:
	// --- 内部逻辑分块 ---
	void UpdateTargetFromAbility();           // 1. 同步：从 Ability 拉取数据
	void UpdateMainBeamVisuals(float DeltaTime);  // 2. 视觉：更新主光束 (带插值)
	void UpdateChainBeamLogic(float DeltaTime);   // 3. 逻辑：闪电链索敌 (低频)
	void UpdateChainBeamVisuals();            // 4. 视觉：闪电链位置 (高频)
	
	// --- 辅助函数 ---
	void CacheSourceAbility();
	void FindAdditionalTargets();
	void UpdateBeamBranches();
	void CleanupAllBranches();

protected:
	// 缓存 Ability 指针
	UPROPERTY()
	TWeakObjectPtr<UAbyssBeamAbility> SourceAbility;

	// 当前锁定的主目标 (从 Ability 同步)
	UPROPERTY()
	TObjectPtr<AActor> TargetActor;
	
	// 施法者
	UPROPERTY()
	TObjectPtr<AActor> SourceActor;

	// 连锁目标缓存列表
	UPROPERTY()
	TArray<AActor*> AdditionalTargets;

	// 连锁光束实例列表
	UPROPERTY()
	TArray<FBeamBranchInfo> BeamBranches;

	// 技能等级 (用于索敌计算)
	int32 AbilityLevel = 1;

	// --- 计时器 ---
	float ChainSearchTimer = 0.0f;
	const float ChainSearchInterval = 0.1f; // 索敌频率 10Hz

	// --- 配置参数 ---
	UPROPERTY(EditDefaultsOnly, Category = "Abyss|Visual")
	TObjectPtr<UNiagaraSystem> BeamSpawnEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Abyss|Visual")
	TObjectPtr<UNiagaraSystem> BeamLoopEffect;
	
	// Niagara 里的 User Parameter 名字 (通常是 "BeamEnd" 或 "Target")
	UPROPERTY(EditDefaultsOnly, Category = "Abyss|Visual")
	FName BeamEndName = FName("BeamEnd");

	// 发射源 Socket (如 "Muzzle_01")，如果不填则从 Root 发射
	UPROPERTY(EditDefaultsOnly, Category = "Abyss|Visual")
	FName BeamSocketName = NAME_None;

	// 默认射程 (当没有目标时)
	UPROPERTY(EditDefaultsOnly, Category = "Abyss|Visual")
	float DefaultTraceDistance = 2000.0f;

	// 连锁范围
	UPROPERTY(EditDefaultsOnly, Category = "Abyss|Logic")
	float ChainRadius = 1000.0f;

	// --- 组件 ---
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> BeamSpawnComponent;
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> BeamLoopComponent;
};