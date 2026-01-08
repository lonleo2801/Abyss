// Copyright (c) 2025 Leon Lee


#include "DebuffNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/AbyssCharacterBase.h"



UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
	bAutoActivate = false;
	
}

void UDebuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (ASC)
	{
		ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
	}
	else if (AAbyssCharacterBase* AbyssCharacter = Cast<AAbyssCharacterBase>(GetOwner()))
	{
		AbyssCharacter->GetOnAscRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* InASC)
		{
			InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
		});
	}
	
	if (AAbyssCharacterBase* AbyssCharacter = Cast<AAbyssCharacterBase>(GetOwner()))
	{
		AbyssCharacter->GetOnDeathDelegate().AddDynamic(this, &UDebuffNiagaraComponent::OnOwnerDeath);
	}
}

void UDebuffNiagaraComponent::DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	const bool bOwnerValid = IsValid(GetOwner());
	const bool bOwnerAlive = !Cast<IAbyssCombatInterface>(GetOwner())->IsDead();
	
	if (NewCount>0 && bOwnerValid && bOwnerAlive)
	{
		Activate();
	}
	else
	{
		Deactivate();
	}
}

void UDebuffNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
	Deactivate();
}


















