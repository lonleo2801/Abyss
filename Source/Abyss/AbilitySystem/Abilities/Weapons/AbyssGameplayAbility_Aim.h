// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AbyssGameplayAbility.h"
#include "AbyssGameplayAbility_Aim.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssGameplayAbility_Aim : public UAbyssGameplayAbility
{
	GENERATED_BODY()
public:
	UAbyssGameplayAbility_Aim();

	// GAS 标准激活入口
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// GAS 标准结束入口 (无论是松手、被眩晕、还是死亡，都会走这里)
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Abyss|Event")
	void BP_PawnEnterWantToAimState();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Abyss|Event")
	void BP_PawnExitWantToAimState();
	
protected:

	// 任务回调：当按键松开时触发
	UFUNCTION()
	void OnInputReleased(float TimeHeld);
};
