// Copyright (c) 2025 Leon Lee


#include "AbyssGameplayAbility.h"

#include "AbyssDebugHelper.h"
#include "AbilitySystem/Attributes/AbyssAttributeSet.h"


void UAbyssGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (bDrawDebugs)
	{
		AbyssDebug::Print(FString::Printf (TEXT("Activated Ability: %s"),*GetName()));
	}
}

FRotator UAbyssGameplayAbility::GetActorRotationSafe() const
{
	if (const AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		return Avatar->GetActorRotation();
	}
	
	return FRotator::ZeroRotator;
}

FRotator UAbyssGameplayAbility::GetControlRotationSafe() const
{
	const AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
	{
		return FRotator::ZeroRotator;
	}

	const APawn* Pawn = Cast<APawn>(Avatar);
	if (Pawn && Pawn->Controller)
	{
		return Pawn->Controller->GetControlRotation();
	}

	return FRotator::ZeroRotator;
}

float UAbyssGameplayAbility::GetManaCost(float InLevel) const
{
	float ManaCost = 0.f;
	if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		for (FGameplayModifierInfo Mod : CostEffect->Modifiers)
		{
			if (Mod.Attribute == UAbyssAttributeSet::GetManaAttribute())
			{
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
				break;
			}
		}
	}
	return ManaCost;
}

float UAbyssGameplayAbility::GetCooldown(float InLevel) const
{
	float Cooldown = 0.f;
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
	}
	return Cooldown;
}


FString UAbyssGameplayAbility::GetDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"), L"Default Ability Name - LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum", Level);
}

FString UAbyssGameplayAbility::GetNextLevelDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Next Level: </><Level>%d</> \n<Default>Causes much more damage. </>"), Level);
}

FString UAbyssGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Spell Locked Until Level: %d</>"), Level);
}

