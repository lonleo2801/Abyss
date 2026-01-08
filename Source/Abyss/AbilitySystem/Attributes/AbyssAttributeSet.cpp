// Copyright (c) 2025 Leon Lee


#include "AbyssAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/BlueprintLibrary/AbyssEffectLibrary.h"
#include "AbilitySystem/Types/AbyssGameplayEffectContext.h"
#include "Components/Combat/AbyssCombatComponentBase.h"
#include "Data/AbyssDataRegistryManager.h"
#include "Data/RegistryTypeDefine.h"
#include "Data/DataTables/AbyssEnemyClassInfoRow.h"
#include "Data/DataTables/AbyssGlobalDataRow.h"
#include "Data/DataTables/AbyssHeroClassInfoRow.h"
#include "GameFramework/Character.h"
#include "GameplayTags/AbyssTags.h"
#include "Interfaces/AbyssEnemyInterface.h"
#include "Net/UnrealNetwork.h"
#include "Player/AbyssPlayerController.h"

UAbyssAttributeSet::UAbyssAttributeSet()
{
	/*InitHealth(100);
	InitMaxHealth(100);
	InitMana(100);
	InitMaxMana(100);
	InitStamina(100);
	InitMaxStamina(100);
	InitPhysicalPower(10);
	InitMagicPower(10);
	InitDefense(10);
	InitCritRate(10);
	InitCritDamage(10);
	InitBlockChance(10);
	InitMagicPenetration(10);
	InitPhysicalPenetration(10);
	InitAttackSpeed(10);
	InitMoveSpeed(10);
	InitCooldownReduction(10);
	InitPhysicalResistance(10);
	InitMagicResistance(10);*/
	

	TagsToAttributesMaps.Add(AbyssTags::Attributes::Health, GetHealthAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::MaxHealth, GetMaxHealthAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::Mana, GetManaAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::MaxMana, GetMaxManaAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::Stamina, GetStaminaAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::MaxStamina, GetMaxStaminaAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::PhysicalPower, GetPhysicalPowerAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::MagicPower, GetMagicPowerAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::Defense, GetDefenseAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::CritRate, GetCritRateAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::CritDamage, GetCritDamageAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::BlockChance, GetBlockChanceAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::MagicPenetration, GetMagicPenetrationAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::PhysicalPenetration, GetPhysicalPenetrationAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::AttackSpeed, GetAttackSpeedAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::MoveSpeed, GetMoveSpeedAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::CooldownReduction, GetCooldownReductionAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::PhysicalResistance, GetPhysicalResistanceAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::MagicResistance, GetMagicResistanceAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::SkillPoints, GetSkillPointsAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::TalentPoints, GetTalentPointsAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::SkillSlotAmount, GetSkillSlotAmountAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::CurrentXP, GetCurrentXPAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::Level, GetLevelAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::XPMultiplier, GetXPMultiplierAttribute);
	
	
	TagsToAttributesMaps.Add(AbyssTags::Attributes::IncomingDamage, GetIncomingDamageAttribute);
	TagsToAttributesMaps.Add(AbyssTags::Attributes::IncomingXP, GetIncomingXPAttribute);
}

void UAbyssAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, PhysicalPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MagicPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Defense, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CritRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CritDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MagicPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, PhysicalPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CooldownReduction, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, PhysicalResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MagicResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, SkillPoints, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, TalentPoints, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, SkillSlotAmount, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CurrentXP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, XPMultiplier, COND_None, REPNOTIFY_Always);
}


void UAbyssAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	//Source = causer of the effect, target = target of the effect(owner of this as)

	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}

		}
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}



void UAbyssAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties EffectProperties;
	SetEffectProperties(Data, EffectProperties);
	
	if (IAbyssCombatInterface* CombatInterface = Cast<IAbyssCombatInterface>(EffectProperties.TargetCharacter))
	{
		if (CombatInterface->IsDead())
		{
			return;
		}
	}

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(EffectProperties);
	}
	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		HandleIncomingXP(EffectProperties);
	}
}

void UAbyssAttributeSet::ShowFloatingText(const FEffectProperties& EffectProperties, float Damage,bool bBlockHit,bool bCritHit) const
{
	if (EffectProperties.SourceCharacter != EffectProperties.TargetCharacter)
	{
		if (AAbyssPlayerController* PC =Cast<AAbyssPlayerController>(EffectProperties.SourceCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage,EffectProperties.TargetCharacter,bBlockHit,bCritHit);
		}
	}
}

void UAbyssAttributeSet::HandleIncomingDamage(const FEffectProperties& EffectProperties)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);
	if (LocalIncomingDamage>0.f)
	{
		if (LocalIncomingDamage==1.f)
		{
			//被格挡了，不造成伤害
		}
		else
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth,0.f, GetMaxHealth()));
			const float bDeath = NewHealth <= 0.f;
			if (bDeath)
			{
				//die and die Impulse
				if (IAbyssCombatInterface* CombatInterface = Cast<IAbyssCombatInterface>(EffectProperties.TargetAvatarActor))
				{
					CombatInterface->Die(UAbyssEffectLibrary::GetDeathImpulse(EffectProperties.EffectContextHandle));
				}
				SendXPEvent(EffectProperties);
			}
			else
			{
				//TODO: Activate hit React
				/*FGameplayTagContainer TagContainer;
				TagContainer.AddTag(.Effects_HitReact);
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);*/
				
				//KnockbackForce
				const FVector& KnockbackForce = UAbyssEffectLibrary::GetKnockbackForce(EffectProperties.EffectContextHandle);
				if (!KnockbackForce.IsNearlyZero(1.f))
				{
					EffectProperties.TargetCharacter->LaunchCharacter(KnockbackForce,true,true);
				}
			}
		}
			
		const bool bBlock = UAbyssEffectLibrary::IsBlockedHit(EffectProperties.EffectContextHandle);
		const bool bCrit = UAbyssEffectLibrary::IsCriticalHit(EffectProperties.EffectContextHandle);
		//TODO: 改为用Niagara来显示伤害数字
		ShowFloatingText(EffectProperties,LocalIncomingDamage,bBlock,bCrit);
		
		//Handle Debuff
		if (UAbyssEffectLibrary::IsSuccessfulDebuff(EffectProperties.EffectContextHandle))
		{
			HandleBuff(EffectProperties);
		}
	}
}

void UAbyssAttributeSet::HandleBuff(const FEffectProperties& EffectProperties)
{
	const FAbyssGlobalDataRow* GlobalDataRow = UAbyssDataRegistryManager::FindCachedSyncByTag<FAbyssGlobalDataRow>(AbyssTags::GlobalData::GlobalData,AbyssRegistryType::GlobalData);
	FGameplayEffectContextHandle EffectContextHandle = EffectProperties.SourceASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(EffectProperties.SourceAvatarActor);
	
	const FGameplayTag DamageType = UAbyssEffectLibrary::GetDamageType(EffectProperties.EffectContextHandle);
	const float DebuffDamage = UAbyssEffectLibrary::GetDebuffDamage(EffectProperties.EffectContextHandle);
	const float DebuffDuration = UAbyssEffectLibrary::GetDebuffDuration(EffectProperties.EffectContextHandle);
	const float DebuffFrequency = UAbyssEffectLibrary::GetDebuffFrequency(EffectProperties.EffectContextHandle);
	
	//DynamicCreate GameplayEffect
	FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"),*DamageType.ToString());
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(),FName(DebuffName));
	//Set Duration
	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Effect->DurationMagnitude = FScalableFloat(DebuffDuration);
	Effect->Period = DebuffFrequency;
	const FGameplayTag DebuffTag = GlobalDataRow->DamageTypesToDebuffMap[DamageType];
	/*新版无法通过这种方式添加tag了：
	Effect->InheritableOwnedTagsContainer.AddTag(DebuffTag);
	要通过：FGameplayEffectSpec->DynamicGrantedTags.AddTag(DebuffTag);
	*/
	
	Effect->StackingType=EGameplayEffectStackingType::AggregateBySource;
	Effect->StackLimitCount = 1;
	//Add Modifier
	const int32 Idx = Effect->Modifiers.Num();
	Effect->Modifiers.Add(FGameplayModifierInfo());
	FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Idx];
	
	ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = GetIncomingDamageAttribute();
	
	//Set Context
	if (FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect,EffectContextHandle,1.f))
	{
		MutableSpec->DynamicGrantedTags.AddTag(DebuffTag);
		FAbyssGameplayEffectContext* AbyssContext = static_cast<FAbyssGameplayEffectContext*>(MutableSpec->GetContext().Get());
		TSharedPtr<FGameplayTag> DebuffDamageType = MakeShareable(new FGameplayTag(DamageType));
		AbyssContext->SetDamageType(DebuffDamageType);
		// Apply
		EffectProperties.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
	}
	
}

void UAbyssAttributeSet::HandleIncomingXP(const FEffectProperties& EffectProperties)
{
	const float LocalIncomingXP = GetIncomingXP();
	SetIncomingXP(0.f);
	
	if (IAbyssCombatInterface* CombatInterface = Cast<IAbyssCombatInterface>(EffectProperties.TargetCharacter))
	{
		int32 CurrentLevel = GetLevel();
		const int32 CurrentXPValue = GetCurrentXP();
		const int32 TotalXP = CurrentXPValue + LocalIncomingXP;
		int32 NewLevel = CurrentLevel;
		
		const FAbyssHeroClassInfoRow* HeroClassInfoRow = 
			UAbyssDataRegistryManager::FindCachedSyncByTag<FAbyssHeroClassInfoRow>(CombatInterface->GetCharacterClass(),AbyssRegistryType::HeroClassInfo);
		
		while (HeroClassInfoRow->LevelUpInfo.LevelUpRequirementXP.GetValueAtLevel(NewLevel+1) <= TotalXP)
		{
			NewLevel++;
		}
		if (NewLevel>CurrentLevel)
		{
			while (NewLevel > CurrentLevel)
			{
				CurrentLevel++;
				const int32 SkillPointsReward = HeroClassInfoRow->LevelUpInfo.SkillPointAward.GetValueAtLevel(CurrentLevel);
				const int32 TalentPointsReward = HeroClassInfoRow->LevelUpInfo.TalentPointAward.GetValueAtLevel(CurrentLevel);
			
				UAbyssEffectLibrary::ApplyAttributeChange(EffectProperties.TargetASC, AbyssTags::Attributes::SkillPoints, SkillPointsReward);
				UAbyssEffectLibrary::ApplyAttributeChange(EffectProperties.TargetASC, AbyssTags::Attributes::TalentPoints, TalentPointsReward);
				
			}
			//更新属性
			UAbyssEffectLibrary::UpdateAllAttributeFromRegistry(EffectProperties.TargetASC,CombatInterface->GetCharacterClass(),NewLevel);
			//更新等级
			UAbyssEffectLibrary::ApplyAttributeChange(EffectProperties.TargetASC, AbyssTags::Attributes::Level, NewLevel, EGameplayModOp::Override);
			CombatInterface->LevelUp(NewLevel);
		}
		//Update CurrentXP
		UAbyssEffectLibrary::ApplyAttributeChange(EffectProperties.TargetASC, AbyssTags::Attributes::CurrentXP, TotalXP,EGameplayModOp::Override);
	}
}

void UAbyssAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	if (Props.TargetCharacter->Implements<UAbyssEnemyInterface>())
	{
		if (IAbyssCombatInterface* CombatInterface = Cast<IAbyssCombatInterface>(Props.TargetCharacter))
		{
			const FAbyssEnemyClassInfoRow* EnemyClassInfoRow =
				UAbyssDataRegistryManager::FindCachedSyncByTag<FAbyssEnemyClassInfoRow>(CombatInterface->GetCharacterClass(),AbyssRegistryType::EnemyClassInfo);
			const int32 EnemyLevel = EnemyClassInfoRow->Level;
			const float XPReward = EnemyClassInfoRow->XPAward.GetValueAtLevel(EnemyLevel);
			
			UAbyssEffectLibrary::ApplyAttributeChange(Props.SourceASC, AbyssTags::Attributes::IncomingXP, XPReward);
		}
	}
}


void UAbyssAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldValue);
}

void UAbyssAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldValue);
}

void UAbyssAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Mana, OldValue);
}

void UAbyssAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxMana, OldValue);
}

void UAbyssAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Stamina, OldValue);
}

void UAbyssAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxStamina, OldValue);
}

void UAbyssAttributeSet::OnRep_PhysicalPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, PhysicalPower, OldValue);
}

void UAbyssAttributeSet::OnRep_MagicPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MagicPower, OldValue);
}

void UAbyssAttributeSet::OnRep_Defense(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Defense, OldValue);
}

void UAbyssAttributeSet::OnRep_CritRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, CritRate, OldValue);
}

void UAbyssAttributeSet::OnRep_CritDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, CritDamage, OldValue);
}

void UAbyssAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, BlockChance, OldValue);
}

void UAbyssAttributeSet::OnRep_MagicPenetration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MagicPenetration, OldValue);
}

void UAbyssAttributeSet::OnRep_PhysicalPenetration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, PhysicalPenetration, OldValue);
}

void UAbyssAttributeSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, AttackSpeed, OldValue);
}

void UAbyssAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MoveSpeed, OldValue);
}

void UAbyssAttributeSet::OnRep_CooldownReduction(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, CooldownReduction, OldValue);
}

void UAbyssAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, PhysicalResistance, OldValue);
}

void UAbyssAttributeSet::OnRep_MagicResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MagicResistance, OldValue);
}

void UAbyssAttributeSet::OnRep_SkillPoints(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, SkillPoints, OldValue);
}

void UAbyssAttributeSet::OnRep_TalentPoints(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, TalentPoints, OldValue);
}

void UAbyssAttributeSet::OnRep_SkillSlotAmount(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, SkillSlotAmount, OldValue);
}

void UAbyssAttributeSet::OnRep_CurrentXP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, CurrentXP, OldValue);
}

void UAbyssAttributeSet::OnRep_Level(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Level, OldValue);
}

void UAbyssAttributeSet::OnRep_XPMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, XPMultiplier, OldValue);
}


