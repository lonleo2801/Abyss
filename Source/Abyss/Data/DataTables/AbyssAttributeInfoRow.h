// Copyright (c) 2025 Leon Lee
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "AbyssAttributeInfoRow.generated.h"




USTRUCT(BlueprintType)
struct FAbyssAttributeInfoRow :public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,meta=(Categories="AbyssTags.Attributes"))
	FGameplayTag AttributeTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeName = FText();
	
	/*UPROPERTY(BlueprintReadOnly)
	float AttributeValue = 0.f;*/
};