// Copyright (c) 2025 Leon Lee
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "AbyssEnemyClassInfoRow.generated.h"


USTRUCT(BlueprintType)
struct FAbyssEnemyClassInfoRow :public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,meta=(Categories="AbyssTags.CharacterClass.Enemy"))
	FGameplayTag EnemyClassTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	int32 Level = 0;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FScalableFloat XPAward;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> InitAttributesEffect;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TArray<FGameplayTag> StartupAbilitiesTags;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag DefaultWeaponTag = FGameplayTag();
};