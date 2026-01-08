// Copyright (c) 2025 Leon Lee
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "AbyssAbilityInfoRow.generated.h"



USTRUCT(BlueprintType)
struct FAbyssAbilityInfoRow :public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,meta=(Categories="AbyssTags.Ability"))
	FGameplayTag AbilityClassTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	int32 LevelRequirement=1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,meta=(Categories="AbyssTags.Cooldown"))
	FGameplayTag CooldownTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,meta=(Categories="AbyssTags.SetByCaller.DamageType"))
	FGameplayTag DamageType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DebuffEffect;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> Ability;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FScalableFloat Damage;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FScalableFloat Cooldown;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FScalableFloat ManaCost;
	
	//升级需要的加点
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FScalableFloat UpgradeRequestPoint;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FText Description;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TObjectPtr<const UTexture2D> Icon = nullptr;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TObjectPtr<const UMaterialInterface> BackgroundMaterial = nullptr;
};


USTRUCT()
struct FAbyssAbilityInfoRow_Runtime :public FTableRowBase
{
	GENERATED_BODY()
	
	bool IsValid() const {return AbilityTag.IsValid();}
	
	//技能不能有两个一样的，用于AddUnique时判断
	bool operator==(const FAbyssAbilityInfoRow_Runtime& Other) const
	{
		return AbilityTag == Other.AbilityTag;
	}
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag AbilityTag = FGameplayTag();
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag DamageType;
	
	UPROPERTY(BlueprintReadOnly)
	float Damage;
	
	//技能装备到快捷栏（槽位）0 代表没装备
	UPROPERTY(BlueprintReadOnly)
	int32 EquipSlot = 0;
	
	UPROPERTY(BlueprintReadOnly)
	int32 Level = 0;
};