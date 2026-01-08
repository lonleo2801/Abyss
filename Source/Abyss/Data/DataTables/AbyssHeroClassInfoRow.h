// Copyright (c) 2025 Leon Lee
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "AbyssHeroClassInfoRow.generated.h"

USTRUCT(BlueprintType)
struct FLevelUpInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FScalableFloat LevelUpRequirementXP;

	UPROPERTY(EditDefaultsOnly)
	FScalableFloat SkillPointAward;

	UPROPERTY(EditDefaultsOnly)
	FScalableFloat TalentPointAward;
};


USTRUCT(BlueprintType)
struct FAbyssHeroClassInfoRow :public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,meta=(Categories="AbyssTags.CharacterClass.Hero"))
	FGameplayTag CharacterClassTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> InitAttributesEffect;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(Categories="AbyssTags.Ability"))
	TArray<FGameplayTag> StartupAbilitiesTags;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FLevelUpInfo LevelUpInfo;
	
	//升级时添加的属性数值
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(Categories="AbyssTags.Attributes"))
	TMap<FGameplayTag,FScalableFloat> AttributesToDifLevelValueMap;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag DefaultWeaponTag = FGameplayTag();
};