// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbyssGameplayAbility.h"
#include "AbilitySystem/Types/AbyssAbilityTypes.h"
#include "AbyssDamageAbility.generated.h"

USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag MontageTag;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag SocketTag;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	USoundBase* ImpactSound = nullptr;
};

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssDamageAbility : public UAbyssGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);
	
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly,Category="Abyss|Damage")
	FGameplayTag DamageType;
	
	UPROPERTY(EditDefaultsOnly,Category="Abyss|Damage")
	FScalableFloat Damage;
	
	//TODO: 把下面这些Debuff的东西也加到RuntimeData和数据注册表上
	UPROPERTY(EditDefaultsOnly,Category="Abyss|Damage")
	float DebuffChance = 50.f;
	
	UPROPERTY(EditDefaultsOnly,Category="Abyss|Damage")
	float DebuffDuration = 5.f;
	
	UPROPERTY(EditDefaultsOnly,Category="Abyss|Damage")
	float DebuffDamage = 5.f;
	
	UPROPERTY(EditDefaultsOnly,Category="Abyss|Damage")
	float DebuffFrequency = 1.f;
	
	UPROPERTY(EditDefaultsOnly,Category="Abyss|Damage")
	float DeathImpulseMagnitude = 1000.f;
	
	UPROPERTY(EditDefaultsOnly,Category="Abyss|Damage")
	float KnockbackForceMagnitude = 1000.f;
	
	UPROPERTY(EditDefaultsOnly,Category="Abyss|Damage")
	float KnockbackChance = 50.f;
	
};
