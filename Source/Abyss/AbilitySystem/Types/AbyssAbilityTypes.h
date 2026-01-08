// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "AbyssAbilityTypes.generated.h"


USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()
	
	FDamageEffectParams(){}
	
	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;
	
	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent = nullptr;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent = nullptr;
	
	UPROPERTY()
	float BaseDamage = 0.f;
	
	UPROPERTY()
	FGameplayTag DamageType = FGameplayTag();
	
	UPROPERTY()
	float DebuffChance = 0.f;
	
	UPROPERTY()
	float DebuffDuration = 0.f;
	
	UPROPERTY()
	float DebuffDamage = 0.f;
	
	UPROPERTY()
	float DebuffFrequency = 0.f;
	
	UPROPERTY()
	float DeathImpulseMagnitude = 0.f;
	
	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;
	
	UPROPERTY()
	float KnockbackForceMagnitude = 0.f;
	
	UPROPERTY()
	FVector KnockbackForce = FVector::ZeroVector;
	
	UPROPERTY()
	float KnockbackChance = 0.f;
	
	UPROPERTY()
	float AbilityLevel = 1.f;
};

















