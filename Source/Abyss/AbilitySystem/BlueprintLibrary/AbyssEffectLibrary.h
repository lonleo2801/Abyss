// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbyssEffectLibrary.generated.h"

struct FDamageEffectParams;
/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssEffectLibrary : public UAbilitySystemBlueprintLibrary
{
	GENERATED_BODY()
public:
	/**
	 *  1.自定义EffectContext的处理
	 */
	
	//Getter
	UFUNCTION(BlueprintPure,Category = "Abyss|AbyssGameplayEffectLibrary|EffectContext")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure,Category = "Abyss|AbyssGameplayEffectLibrary|EffectContext")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure, Category = "Abyss|AbyssGameplayEffectLibrary|EffectContext")
	static bool IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure, Category = "Abyss|AbyssGameplayEffectLibrary|EffectContext")
	static float GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "Abyss|AbyssGameplayEffectLibrary|EffectContext")
	static float GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "Abyss|AbyssGameplayEffectLibrary|EffectContext")
	static float GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "Abyss|AbyssGameplayEffectLibrary|EffectContext")
	static FGameplayTag GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "Abyss|AbyssGameplayEffectLibrary|EffectContext")
	static FVector GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure, Category = "Abyss|AbyssGameplayEffectLibrary|EffectContext")
	static FVector GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle);
	//Setter
	
	UFUNCTION(BlueprintCallable,Category = "Abyss|AbyssGameplayEffectLibrary|EffectContext")
	//加上UPARAM(ref)，是让函数参数变为输入引脚，而不是输出引脚
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);

	UFUNCTION(BlueprintCallable,Category = "Abyss|AbyssGameplayEffectLibrary|EffectContext")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);
	
	UFUNCTION(BlueprintCallable, Category = "Abyss|AbyssGameplayEffectLibrary|EffectContext")
	static void SetIsSuccessfulDebuff(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bIsSuccessfulDebuff);
	
	UFUNCTION(BlueprintCallable, Category = "Abyss|AbyssGameplayEffectLibrary|EffectContext")
	static void SetDebuffDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InDamage);

	UFUNCTION(BlueprintCallable, Category = "Abyss|AbyssGameplayEffectLibrary|EffectContext")
	static void SetDebuffDuration(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InDuration);

	UFUNCTION(BlueprintCallable, Category = "Abyss|AbyssGameplayEffectLibrary|EffectContext")
	static void SetDebuffFrequency(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InFrequency);

	UFUNCTION(BlueprintCallable, Category = "Abyss|AbyssGameplayEffectLibrary|EffectContext")
	static void SetDamageType(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType);

	UFUNCTION(BlueprintCallable, Category = "Abyss|AbyssGameplayEffectLibrary|EffectContext")
	static void SetDeathImpulse(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InDeathImpulse);
	
	UFUNCTION(BlueprintCallable, Category = "Abyss|AbyssGameplayEffectLibrary|EffectContext")
	static void SetKnockbackForce(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InKnockbackForce);
	
	/**
	 *  2.FDamageEffectParams的Apply
	 */
	UFUNCTION(BlueprintCallable,Category = "Abyss|AbyssGameplayEffectLibrary|DamageEffect")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams);
	
	
	/**
	* 3.应用对应属性的Instant GameplayEffect 到目标 ASC
	* @param TargetASC 目标 Ability System Component
	* @param AttributeTag 属性Tag
	* @param Magnitude 属性修改值
	* @return 是否成功
	*/
	UFUNCTION(Category = "Abyss|AbyssGameplayEffectLibrary|GameplayEffects")
	static bool ApplyAttributeChange(UAbilitySystemComponent* TargetASC,const FGameplayTag& AttributeTag, float Magnitude, EGameplayModOp::Type Op = EGameplayModOp::AddBase);
	
	UFUNCTION(BlueprintCallable, Category = "Abyss|AbyssGameplayEffectLibrary|GameplayEffects")
	static bool UpdateAllAttributeFromRegistry(UAbilitySystemComponent* TargetASC,const FGameplayTag& CharacterClassTag, int32 NewLevel);
	
	UFUNCTION(BlueprintCallable, Category = "Abyss|AbyssGameplayEffectLibrary|GameplayEffects")
	static bool ApplySkillPointsChange(UAbilitySystemComponent* TargetASC, int32 PointDelta);
	
	
	
};
