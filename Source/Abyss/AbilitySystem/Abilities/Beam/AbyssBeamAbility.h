// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AbyssDamageAbility.h"
#include "AbyssBeamAbility.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssBeamAbility : public UAbyssDamageAbility
{
	GENERATED_BODY()
	
public:
	UAbyssBeamAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	// 这个变量供 GameplayCue 读取
	UPROPERTY(BlueprintReadOnly, Category = "Beam")
	TObjectPtr<AActor> CurrentHitTarget;
protected:
	// 处理 TargetData 回调（核心循环）
	UFUNCTION()
	void OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle);

	// 处理松开按键
	UFUNCTION()
	void OnReleaseInput(float TimeHeld);

	// 辅助：造成连锁伤害
	void ApplyChainDamage(AActor* MainTarget);
	
	// 当 Start 蒙太奇自然结束（或 BlendOut）时调用，用于衔接 Loop
	UFUNCTION()
	void OnBeamStartMontageEnded();

	// 当 Start 蒙太奇被打断或取消时调用
	UFUNCTION()
	void OnBeamStartMontageFailed();
	
	// 通常 Loop 是无限循环的，这个函数主要用于处理"被打断"的情况
	UFUNCTION()
	void OnBeamLoopMontageEnded();
	
	UFUNCTION()
	void OnBeamEndMontageEnded();

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Beam")
	TObjectPtr<UAnimMontage> BeamStartMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Beam")
	TObjectPtr<UAnimMontage> BeamLoopMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Beam")
	TObjectPtr<UAnimMontage> BeamEndMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Beam")
	FGameplayTag BeamCueTag;

	// 索敌参数
	UPROPERTY(EditDefaultsOnly, Category = "Beam")
	float ChainRadius = 1000.f;
	
	// 可以在蓝图或数据表中配置的频率，例如 0.1s 或 0.2s
	UPROPERTY(EditDefaultsOnly, Category = "Beam")
	float DamageAndHitTickInterval = 0.1f;

private:
	// 缓存当前的 Cue 参数，避免重复移除/添加
	FGameplayCueParameters CurrentCueParams;
};

