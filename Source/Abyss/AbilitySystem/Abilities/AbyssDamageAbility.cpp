// Copyright (c) 2025 Leon Lee


#include "AbyssDamageAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/BlueprintLibrary/AbyssAbilitySystemLibrary.h"

void UAbyssDamageAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	UAbyssAbilitySystemLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageType, ScaledDamage);
	
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FDamageEffectParams UAbyssDamageAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	//TODO: 可以从数据注册表读取
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	//TODO: 可以改为从RuntimeData读取设置参数
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDuration = DebuffDuration;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockbackForceMagnitude = KnockbackForceMagnitude;
	Params.KnockbackChance = KnockbackChance;
	Params.AbilityLevel = GetAbilityLevel();
	
	if (IsValid(TargetActor))//目标有效，说明不是发射Projectile的技能，在这里设置击退
	{
		FRotator ToTargetRotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		ToTargetRotation.Pitch = FMath::RandRange(20.f, 70.f);
		const FVector ToTarget = ToTargetRotation.Vector();
		Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;
		Params.KnockbackForce = ToTarget * KnockbackForceMagnitude;
	}
	
	return Params;
}


















