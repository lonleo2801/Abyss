// Copyright (c) 2025 Leon Lee


#include "AbyssEffectLibrary.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "AbilitySystem/Attributes/AbyssAttributeSet.h"
#include "AbilitySystem/Types/AbyssAbilityTypes.h"
#include "AbilitySystem/Types/AbyssGameplayEffectContext.h"
#include "Characters/AbyssCharacterBase.h"
#include "Data/AbyssDataRegistryManager.h"
#include "Data/RegistryTypeDefine.h"
#include "Data/DataTables/AbyssGlobalDataRow.h"
#include "Data/DataTables/AbyssHeroClassInfoRow.h"
#include "GameplayTags/AbyssTags.h"
#include "Interfaces/AbyssCombatInterface.h"


bool UAbyssEffectLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAbyssGameplayEffectContext* AbyssEffectContext = static_cast<const FAbyssGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AbyssEffectContext->IsBlockedHit();
	}
	return false;
}

bool UAbyssEffectLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAbyssGameplayEffectContext* AbyssEffectContext = static_cast<const FAbyssGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AbyssEffectContext->IsCriticalHit();
	}
	return false;
}

bool UAbyssEffectLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAbyssGameplayEffectContext* AbyssEffectContext = static_cast<const FAbyssGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AbyssEffectContext->IsSuccessfulDebuff();
	}
	return false;
}


float UAbyssEffectLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAbyssGameplayEffectContext* AbyssEffectContext = static_cast<const FAbyssGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AbyssEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UAbyssEffectLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAbyssGameplayEffectContext* AbyssEffectContext = static_cast<const FAbyssGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AbyssEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

float UAbyssEffectLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAbyssGameplayEffectContext* AbyssEffectContext = static_cast<const FAbyssGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AbyssEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

FGameplayTag UAbyssEffectLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAbyssGameplayEffectContext* AbyssEffectContext = static_cast<const FAbyssGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (AbyssEffectContext->GetDamageType().IsValid())
		{
			return *AbyssEffectContext->GetDamageType();
		}
	}
	return FGameplayTag();
}

FVector UAbyssEffectLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAbyssGameplayEffectContext* AbyssEffectContext = static_cast<const FAbyssGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AbyssEffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

FVector UAbyssEffectLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAbyssGameplayEffectContext* AbyssEffectContext = static_cast<const FAbyssGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AbyssEffectContext->GetKnockbackForce();
	}
	return FVector::ZeroVector;
}


void UAbyssEffectLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle,bool bInIsBlockedHit)
{
	if (FAbyssGameplayEffectContext* AbyssEffectContext = static_cast<FAbyssGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AbyssEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UAbyssEffectLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,bool bInIsCriticalHit)
{
	if (FAbyssGameplayEffectContext* AbyssEffectContext = static_cast<FAbyssGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AbyssEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UAbyssEffectLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle,bool bIsSuccessfulDebuff)
{
	if (FAbyssGameplayEffectContext* AbyssEffectContext = static_cast<FAbyssGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AbyssEffectContext->SetIsSuccessfulDebuff(bIsSuccessfulDebuff);
	}
}

void UAbyssEffectLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float InDamage)
{
	if (FAbyssGameplayEffectContext* AbyssEffectContext = static_cast<FAbyssGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AbyssEffectContext->SetDebuffDamage(InDamage);
	}
}

void UAbyssEffectLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float InDuration)
{
	if (FAbyssGameplayEffectContext* AbyssEffectContext = static_cast<FAbyssGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AbyssEffectContext->SetDebuffDuration(InDuration);
	}
}

void UAbyssEffectLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, float InFrequency)
{
	if (FAbyssGameplayEffectContext* AbyssEffectContext = static_cast<FAbyssGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AbyssEffectContext->SetDebuffFrequency(InFrequency);
	}
}

void UAbyssEffectLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle,const FGameplayTag& InDamageType)
{
	if (FAbyssGameplayEffectContext* AbyssEffectContext = static_cast<FAbyssGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		AbyssEffectContext->SetDamageType(DamageType);
	}
}

void UAbyssEffectLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle,const FVector& InDeathImpulse)
{
	if (FAbyssGameplayEffectContext* AbyssEffectContext = static_cast<FAbyssGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AbyssEffectContext->SetDeathImpulse(InDeathImpulse);
	}
}

void UAbyssEffectLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle,const FVector& InKnockbackForce)
{
	if (FAbyssGameplayEffectContext* AbyssEffectContext = static_cast<FAbyssGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AbyssEffectContext->SetKnockbackForce(InKnockbackForce);
	}
}

FGameplayEffectContextHandle UAbyssEffectLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	SetDeathImpulse(EffectContextHandle, DamageEffectParams.DeathImpulse);
	SetKnockbackForce(EffectContextHandle, DamageEffectParams.KnockbackForce);
	
	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(
			DamageEffectParams.DamageGameplayEffectClass,
			DamageEffectParams.AbilityLevel,
			EffectContextHandle
			);
	
	// 设置SetByCaller
	AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	AssignTagSetByCallerMagnitude(SpecHandle, AbyssTags::SetByCaller::Debuff::Chance, DamageEffectParams.DebuffChance);
	AssignTagSetByCallerMagnitude(SpecHandle, AbyssTags::SetByCaller::Debuff::Duration, DamageEffectParams.DebuffDuration);
	AssignTagSetByCallerMagnitude(SpecHandle, AbyssTags::SetByCaller::Debuff::Damage, DamageEffectParams.DebuffDamage);
	AssignTagSetByCallerMagnitude(SpecHandle, AbyssTags::SetByCaller::Debuff::Frequency, DamageEffectParams.DebuffFrequency);
	
	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	
	return EffectContextHandle;
}

bool UAbyssEffectLibrary::ApplyAttributeChange(UAbilitySystemComponent* TargetASC, const FGameplayTag& AttributeTag,float Magnitude, EGameplayModOp::Type Op)
{
	if (!TargetASC)
		return false;
	const UAbyssAttributeSet* AttributeSet = TargetASC->GetSet<UAbyssAttributeSet>();
	
	//DynamicCreate GameplayEffect
	FString EffectName = FString::Printf(TEXT("DynamicDebuff_%s"),*AttributeTag.ToString());
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(),FName(EffectName));
	
	//Add Modifier
	const int32 Idx = Effect->Modifiers.Num();
	Effect->Modifiers.Add(FGameplayModifierInfo());
	FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Idx];
	
	ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(Magnitude);
	ModifierInfo.ModifierOp = Op;
	for (auto& Pair : AttributeSet->TagsToAttributesMaps)
	{
		if (Pair.Key==AttributeTag)
		{
			ModifierInfo.Attribute = Pair.Value();
		}
	}
	
	//Set Context
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	if (FGameplayEffectSpec* Spec = new FGameplayEffectSpec(Effect,EffectContextHandle,1.f))
	{
		// Apply
		TargetASC->ApplyGameplayEffectSpecToSelf(*Spec);
		return true;
	}
	return false;
}

bool UAbyssEffectLibrary::UpdateAllAttributeFromRegistry(UAbilitySystemComponent* TargetASC,const FGameplayTag& CharacterClassTag,int32 NewLevel)
{
	if (!TargetASC)
		return false;
	
	const FAbyssHeroClassInfoRow* HeroClassInfoRow = 
		UAbyssDataRegistryManager::FindCachedSyncByTag<FAbyssHeroClassInfoRow>(CharacterClassTag,AbyssRegistryType::HeroClassInfo);
	if (HeroClassInfoRow)
	{
		for (auto& Pair :  HeroClassInfoRow->AttributesToDifLevelValueMap)
		{
			ApplyAttributeChange(TargetASC, Pair.Key, Pair.Value.GetValueAtLevel(NewLevel),EGameplayModOp::AddBase);
		}
		return true;
	}
	return false;

}


bool UAbyssEffectLibrary::ApplySkillPointsChange(UAbilitySystemComponent* TargetASC, int32 PointDelta)
{
	return ApplyAttributeChange(TargetASC, AbyssTags::Attributes::SkillPoints, PointDelta);
}

