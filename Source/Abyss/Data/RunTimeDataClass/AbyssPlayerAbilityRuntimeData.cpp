// Copyright (c) 2025 Leon Lee


#include "AbyssPlayerAbilityRuntimeData.h"

#include "AbyssDebugHelper.h"
#include "AbilitySystem/AbyssAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/AbyssAttributeSet.h"
#include "AbilitySystem/BlueprintLibrary/AbyssEffectLibrary.h"
#include "Characters/Hero/Character/AbyssHeroBase.h"
#include "Data/AbyssDataRegistryManager.h"
#include "Data/RegistryTypeDefine.h"
#include "Data/DataTables/AbyssHeroClassInfoRow.h"
#include "GameplayTags/AbyssTags.h"



void UAbyssPlayerAbilityRuntimeData::Initialize(const AAbyssHeroBase* HeroCharacter,FGameplayTag CharacterTypeTag)
{
	Reset();
	AbilitySystemComponent = HeroCharacter->GetAbilitySystemComponent();
	AttributeSet = Cast<UAbyssAttributeSet>(HeroCharacter->GetAttributeSet());
	
	//TODO:临时定义，后面搞好按键映射回来重新做初始化
	TArray<FGameplayTag> InputTags;
	InputTags.Add(FGameplayTag());
	InputTags.Add(AbyssTags::InputTag::Q);
	InputTags.Add(AbyssTags::InputTag::E);
	InputTags.Add(AbyssTags::InputTag::R);
	InputTags.Add(AbyssTags::InputTag::F);
	
	DamageTypesToDebuffMap.Add(AbyssTags::SetByCaller::DamageType::Physical,AbyssTags::SetByCaller::Debuff::Physical);
	DamageTypesToDebuffMap.Add(AbyssTags::SetByCaller::DamageType::Fire,AbyssTags::SetByCaller::Debuff::Fire);
	DamageTypesToDebuffMap.Add(AbyssTags::SetByCaller::DamageType::Lightning,AbyssTags::SetByCaller::Debuff::Lightning);
	DamageTypesToDebuffMap.Add(AbyssTags::SetByCaller::DamageType::Ice,AbyssTags::SetByCaller::Debuff::Ice);
	DamageTypesToDebuffMap.Add(AbyssTags::SetByCaller::DamageType::Poison,AbyssTags::SetByCaller::Debuff::Poison);

	if (AttributeSet)
	{
		for (int i = 1; i <= AttributeSet->GetSkillSlotAmount(); ++i)
		{
			SlotInputTagMap.Add(i,InputTags[i]);
		}
	}
	
	const FAbyssHeroClassInfoRow* CharacterClassInfo =
		UAbyssDataRegistryManager::FindCachedSyncByTag<FAbyssHeroClassInfoRow>(CharacterTypeTag, AbyssRegistryType::HeroClassInfo);
	
	if (CharacterClassInfo)
	{
		TArray<FGameplayTag> StartupAbilityTags(CharacterClassInfo->StartupAbilitiesTags);
		UAbyssAbilitySystemComponent *AbyssASC = Cast<UAbyssAbilitySystemComponent>(AbilitySystemComponent);
		TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
		for (const FGameplayTag& AbilityTag : StartupAbilityTags)
		{
			const FAbyssAbilityInfoRow* AbilityStaticInfo =
				UAbyssDataRegistryManager::FindCachedSyncByTag<FAbyssAbilityInfoRow>(AbilityTag, AbyssRegistryType::AbilityInfo);
			
			if (AbilityStaticInfo)
			{
				//添加到初始化startup技能数组
				StartupAbilities.Add(AbilityStaticInfo->Ability);
				
				//初始化Runtime技能数据
				FAbyssAbilityInfoRow_Runtime AbilityRuntimeInfo;
				AbilityRuntimeInfo.AbilityTag = AbilityStaticInfo->AbilityClassTag;
				AbilityRuntimeInfo.DamageType = AbilityStaticInfo->DamageType;
				RuntimeAbilityInformation.AddUnique(AbilityRuntimeInfo);
				//TODO: 可以从保存的数据里面读取以前的等级是多少
				ModifyAbilityLevel(AbilityTag,1);
			}
		}
		//初始化startup技能
		AbyssASC->AddCharacterStartupAbilities(StartupAbilities);
	}
}

void UAbyssPlayerAbilityRuntimeData::Reset()
{
	RuntimeAbilityInformation.Reset();
}

void UAbyssPlayerAbilityRuntimeData::LearningAbility(const FGameplayTag& AbilityTag)
{
	if (!AbilitySystemComponent)
	{
		const FString ErrorMessage = FString::Printf(TEXT("RuntimeData: No ASC in Reset!"));
		AbyssDebug::Print(ErrorMessage);
		return;
	}

	const FAbyssAbilityInfoRow* StaticInfo = UAbyssDataRegistryManager::FindCachedSyncByTag<FAbyssAbilityInfoRow>(AbilityTag, AbyssRegistryType::AbilityInfo);
	if (!StaticInfo)
	{
		const FString ErrorMessage = FString::Printf(TEXT("RuntimeData: Ability %s not found in DataRegistry!"), *AbilityTag.ToString());	
		AbyssDebug::Print(ErrorMessage);
		return;
	}

	// 查找是否已存在 Runtime 信息
	FAbyssAbilityInfoRow_Runtime RuntimeInfo = FindAbilityInfoForTag(AbilityTag);
	if (RuntimeInfo.IsValid())
	{
		const FString ErrorMessage = FString::Printf(TEXT("RuntimeData: Ability %s already learned!"), *AbilityTag.ToString());
		AbyssDebug::Print(ErrorMessage);
		return;
	}
	else
	{
		// 创建新的 Runtime 条目
		FAbyssAbilityInfoRow_Runtime NewEntry;
		NewEntry.AbilityTag = AbilityTag;
		NewEntry.DamageType = StaticInfo->DamageType;
		RuntimeAbilityInformation.Add(NewEntry);
		ModifyAbilityLevel(AbilityTag,1);
		
		// ----------------------------
		// 给予 Ability 给 ASC
		// ----------------------------
		if (StaticInfo->Ability)
		{
			FGameplayAbilitySpec Spec(StaticInfo->Ability, NewEntry.Level);
			AbilitySystemComponent->GiveAbility(Spec);
		}
		else
		{
			const FString ErrorMessage = FString::Printf(TEXT("RuntimeData: Ability %s not found in DataRegistry!"), *AbilityTag.ToString());
			AbyssDebug::Print(ErrorMessage);
		}
	}

	// 若需要，这里可以发 delegate 给 ViewModel
	//OnAbilityLearned.Broadcast(AbilityTag);

	//UE_LOG(LogTemp, Log, TEXT("RuntimeData: Learned ability %s"), *AbilityTag.ToString());

}

void UAbyssPlayerAbilityRuntimeData::EquipAbility(const FGameplayTag& AbilityTag, int32 Slot)
{
	if (!AbilitySystemComponent)
	{
		const FString ErrorMessage = FString::Printf(TEXT("RuntimeData: No ASC in EquipAbility!"));
		AbyssDebug::Print(ErrorMessage);
		return;
	}

	if (Slot < 1 || Slot > AttributeSet->GetSkillSlotAmount())
	{
		const FString ErrorMessage = FString::Printf(TEXT("RuntimeData: Invalid Slot Index %d"), Slot);
		AbyssDebug::Print(ErrorMessage);
		return;
	}

	// 查找技能学了没有
	FAbyssAbilityInfoRow_Runtime NewAbilityInfo = FindAbilityInfoForTag(AbilityTag);
	if (!NewAbilityInfo.IsValid())
	{
		const FString ErrorMessage = FString::Printf(TEXT("RuntimeData: Ability %s not learned, cannot equip!"), *AbilityTag.ToString());
		AbyssDebug::Print(ErrorMessage);
		return;
	}

	// 如果该槽位已有技能 → 先卸载
	FAbyssAbilityInfoRow_Runtime* OldAbilityInfo = RuntimeAbilityInformation.FindByPredicate(
		[&](const FAbyssAbilityInfoRow_Runtime& Item) { return Item.EquipSlot == Slot; });
	if (OldAbilityInfo)
	{
		FGameplayTag OldTag = OldAbilityInfo->AbilityTag;
		UnEquipAbility(OldTag, Slot);
	}
	
	// 装备新技能
	NewAbilityInfo.EquipSlot = Slot;
	NewAbilityInfo.InputTag = SlotInputTagMap[Slot];
	//可以broadcast给viewmodel
	// 触发 UI 事件
	//OnAbilityEquipped.Broadcast(AbilityTag, Slot);
	

}

void UAbyssPlayerAbilityRuntimeData::UnEquipAbility(const FGameplayTag& AbilityTag, int32 Slot)
{
	if (!AbilitySystemComponent)
		return;

	FAbyssAbilityInfoRow_Runtime* RuntimeInfo = RuntimeAbilityInformation.FindByPredicate(
		[&](const FAbyssAbilityInfoRow_Runtime& Item) { return Item.AbilityTag == AbilityTag; });

	if (RuntimeInfo)
	{
		RuntimeInfo->EquipSlot = 0;
		RuntimeInfo->InputTag = FGameplayTag();
	}

	//可以broadcast给viewmodel
	// 触发 UI 事件
	//OnAbilityUnEquipped.Broadcast(AbilityTag, Slot);
}


void UAbyssPlayerAbilityRuntimeData::ResetOneAbility(const FGameplayTag& AbilityTag)
{
	FAbyssAbilityInfoRow_Runtime RuntimeInfo = FindAbilityInfoForTag(AbilityTag);
	if (!RuntimeInfo.IsValid())
		return;
	// ASC撤销tag对应的ability
	for (FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(AbilityTag))
		{
			ModifyAbilityLevel(AbilityTag,RuntimeInfo.Level);
			AbilitySystemComponent->ClearAbility(Spec.Handle);
			break;
		}
	}
	RuntimeAbilityInformation.Remove(RuntimeInfo);
	
	//TODO: Broadcast To Spell Menu 
}

void UAbyssPlayerAbilityRuntimeData::ResetAllAbility()
{
	UAbyssAbilitySystemComponent* AbyssASC = Cast<UAbyssAbilitySystemComponent>(AbilitySystemComponent);
	
	// ASC撤销所有带ability的能力
	for (FGameplayAbilitySpec& Spec : AbyssASC->GetActivatableAbilities())
	{
		FGameplayTag Tag = AbyssASC->GetAbilityTagFromSpec(Spec);
		if (Tag.IsValid())
		{
			FAbyssAbilityInfoRow_Runtime RuntimeInfo = FindAbilityInfoForTag(Tag);
			ModifyAbilityLevel(Tag,-RuntimeInfo.Level);
			AbyssASC->ClearAbility(Spec.Handle);
		}
	}
	RuntimeAbilityInformation.Reset();
	
	
	//TODO: Broadcast To Spell Menu, Tell AttributeSet return all upgrade point
}


/**
 * 升级或者降级并且处理对应技能点数修改
 */
bool UAbyssPlayerAbilityRuntimeData::ModifyAbilityLevel(const FGameplayTag& AbilityTag,int32 DeltaLevel)
{
	UAbyssAbilitySystemComponent* AbyssASC = Cast<UAbyssAbilitySystemComponent>(AbilitySystemComponent);

	if (!AbyssASC || DeltaLevel == 0)
		return false;

	FAbyssAbilityInfoRow_Runtime RuntimeInfo = FindAbilityInfoForTag(AbilityTag);
	if (!RuntimeInfo.IsValid())
		return false;

	const FAbyssAbilityInfoRow* StaticInfo =UAbyssDataRegistryManager::FindCachedSyncByTag<FAbyssAbilityInfoRow>(AbilityTag, AbyssRegistryType::AbilityInfo);
	if (!StaticInfo)
		return false;

	int32 OldLevel = RuntimeInfo.Level;
	int32 TargetLevel = FMath::Max(0, OldLevel + DeltaLevel);
	int32 PointDelta = 0;

	if (DeltaLevel > 0)
	{
		// ====================
		//      升级计算
		// ====================
		for (int32 L = OldLevel + 1; L <= TargetLevel; ++L)
		{
			PointDelta -= StaticInfo->UpgradeRequestPoint.GetValueAtLevel(L);
		}
	}
	else
	{
		// ====================
		//      降级计算
		// ====================
		for (int32 L = OldLevel; L > TargetLevel; --L)
		{
			PointDelta += StaticInfo->UpgradeRequestPoint.GetValueAtLevel(L);
		}
	}

	// 修改技能等级与伤害
	RuntimeInfo.Level = TargetLevel;
	RuntimeInfo.Damage = StaticInfo->Damage.GetValueAtLevel(TargetLevel);
	// 应用到ability
	AbyssASC->SetAbilityLevel(StaticInfo->Ability, TargetLevel);

	// 应用技能点变化（正 = 返还；负 = 扣除）
	if (PointDelta != 0)
	{
		UAbyssEffectLibrary::ApplySkillPointsChange(AbyssASC, PointDelta);
	}

	return true;
}


FAbyssAbilityInfoRow_Runtime UAbyssPlayerAbilityRuntimeData::FindAbilityInfoForTag(const FGameplayTag& AbilityTag) const
{
	for (const FAbyssAbilityInfoRow_Runtime& Info:RuntimeAbilityInformation)
	{
		if (Info.AbilityTag==AbilityTag)
		{
			return Info;
		}
	}
	AbyssDebug::Print("Can't find info for AbilityTag");
	return FAbyssAbilityInfoRow_Runtime();
}
