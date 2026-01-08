// Copyright (c) 2025 Leon Lee
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "AbyssGlobalDataRow.generated.h"


class UGameplayEffect;
/*
 *存储一些全局变量
 */
USTRUCT(BlueprintType)
struct FAbyssGlobalDataRow :public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,meta=(Categories="AbyssTags.SetByCaller"))
	TMap<FGameplayTag,FGameplayTag> DamageTypesToDebuffMap;
	
	UPROPERTY(EditDefaultsOnly)
	//加法的GameplayEffectTemplate
	TSubclassOf<UGameplayEffect> InstantAdditionsEffectTemplate;
};
