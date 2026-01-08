// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbyssGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** 获取角色当前的世界朝向（Actor Rotation） */
	UFUNCTION(BlueprintCallable, Category="Abyss|Ability")
	FRotator GetActorRotationSafe() const;

	/** 获取控制器的朝向（Control Rotation） */
	UFUNCTION(BlueprintCallable, Category="Abyss|Ability")
	FRotator GetControlRotationSafe() const;

protected:
	float GetManaCost(float InLevel = 1.f) const;
	float GetCooldown(float InLevel = 1.f) const;	
	
public:	
	
	UPROPERTY(EditDefaultsOnly,Category= "Abyss|Input")
	FGameplayTag StartupInputTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abyss|Debug")
	bool bDrawDebugs = false;
	
public:
	//TODO:  把技能描述移到数据注册表上
	virtual FString GetDescription(int32 Level);
	virtual FString GetNextLevelDescription(int32 Level);
	static FString GetLockedDescription(int32 Level);
	
	
public:
	
};
