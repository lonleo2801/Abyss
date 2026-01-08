// Copyright (c) 2025 Leon Lee
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "AbyssDamageTypeInfoRow.generated.h"




USTRUCT(BlueprintType)
struct FAbyssDamageTypeInfoRow :public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,meta=(Categories="AbyssTags.SetByCaller.DamageType"))
	FGameplayTag AttributeTag = FGameplayTag();
	
	//显示状态栏图标，文字
	
	//伤害数字的颜色
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor DamageColor = FLinearColor::White;
};