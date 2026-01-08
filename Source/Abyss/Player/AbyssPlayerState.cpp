// Copyright (c) 2025 Leon Lee


#include "AbyssPlayerState.h"
#include "AbilitySystem/AbyssAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/AbyssAttributeSet.h"
#include "Characters/Hero/Character/AbyssHeroBase.h"

AAbyssPlayerState::AAbyssPlayerState()
{
	SetNetUpdateFrequency(100.f);

	AbilitySystemComponent = CreateDefaultSubobject<UAbyssAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AbilitySystemComponent->SetIsReplicated(true);
	
	AttributeSet = CreateDefaultSubobject<UAbyssAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* AAbyssPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAbyssPlayerState::InitRuntimeData(AAbyssHeroBase* Hero,FGameplayTag CharacterTypeTag)
{
	RuntimePlayerAbilityData = NewObject<UAbyssPlayerAbilityRuntimeData>(this);
	RuntimePlayerAbilityData->Initialize(Hero,CharacterTypeTag);
}

void AAbyssPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
}

