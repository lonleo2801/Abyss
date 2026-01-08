// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AbyssAbilitySystemComponent.h"
#include "Data/DataTables/AbyssAbilityInfoRow.h"
#include "UObject/Object.h"
#include "AbyssPlayerAbilityRuntimeData.generated.h"


class AAbyssHeroBase;
class UAbyssAttributeSet;
/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssPlayerAbilityRuntimeData : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(const AAbyssHeroBase* HeroCharacter,FGameplayTag CharacterTypeTag);
	void Reset();
	void LearningAbility(const FGameplayTag& AbilityTag);
	void EquipAbility(const FGameplayTag& AbilityTag,int32 Slot);
	void UnEquipAbility(const FGameplayTag& AbilityTag,int32 Slot);
	//技能重置
	void ResetOneAbility(const FGameplayTag& AbilityTag);
	//洗点
	void ResetAllAbility();
	//升级或者降级并且处理对应技能点数修改
	bool ModifyAbilityLevel(const FGameplayTag& AbilityTag,int32 DeltaLevel);
	
	FAbyssAbilityInfoRow_Runtime FindAbilityInfoForTag(const FGameplayTag& AbilityTag) const;
	
	
	UPROPERTY()
	TArray<FAbyssAbilityInfoRow_Runtime> RuntimeAbilityInformation;
	
	//TODO: 临时定义
	UPROPERTY()
	TMap<int32,FGameplayTag> SlotInputTagMap;
	
	UPROPERTY()
	TMap<FGameplayTag,FGameplayTag> DamageTypesToDebuffMap;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;
	
	UPROPERTY()
	TObjectPtr<UAbyssAttributeSet> AttributeSet = nullptr;
	
public:
	
};
