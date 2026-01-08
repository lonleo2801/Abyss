// Copyright (c) 2025 Leon Lee


#include "AbyssWaitCooldownChange.h"

#include "AbilitySystemComponent.h"

UAbyssWaitCooldownChange* UAbyssWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCooldownTag)
{
	UAbyssWaitCooldownChange* WaitCooldownChange = NewObject<UAbyssWaitCooldownChange>();
	WaitCooldownChange->ASC = AbilitySystemComponent;
	WaitCooldownChange->CooldownTag = InCooldownTag;
	
	if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
	{
		WaitCooldownChange->EndTask();
		return nullptr;
	}
	
	//To know when a cooldown has ended (Cooldown Tag has been removed) 
	AbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(WaitCooldownChange, &UAbyssWaitCooldownChange::OnCooldownTagChanged);
	
	//To know when a cooldown effect has been applied
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange, &UAbyssWaitCooldownChange::OnActiveEffectAdded);
	
	return WaitCooldownChange;
}

void UAbyssWaitCooldownChange::EndTask()
{
	if (IsValid(ASC)) return;

	ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	
	SetReadyToDestroy();
	MarkAsGarbage();
}

void UAbyssWaitCooldownChange::OnCooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount == 0) // Tag 的数量为0，说明被移除了，冷却结束
	{ 
		OnCooldownEnd.Broadcast(0.f);
	}
}

void UAbyssWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetAsc,const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);
	
	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);
	
	if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
	{
		FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		TArray<float> CooldownsRemaining = ASC->GetActiveEffectsTimeRemaining(Query);
		if (CooldownsRemaining.Num()>0)
		{
			float CooldownRemaining = CooldownsRemaining[0];
			for (int32 i = 1; i < CooldownsRemaining.Num(); i++)
			{
				if (CooldownsRemaining[i] > CooldownRemaining)
				{
					CooldownRemaining = CooldownsRemaining[i];
				}
			}
			OnCooldownStart.Broadcast(CooldownRemaining);
		}
	}
}










