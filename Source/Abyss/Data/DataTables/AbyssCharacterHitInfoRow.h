// Copyright (c) 2025 Leon Lee
#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Engine/DataTable.h"
#include "AbyssCharacterHitInfoRow.generated.h"




USTRUCT(BlueprintType)
struct FAbyssCharacterHitInfoRow :public FTableRowBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, Category = "CharacterStyle")
	int32 ID;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Effect")
	TArray<UNiagaraSystem*> BiologyHit;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Effect")
	TArray<USoundBase*> ReboundSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Effect")
	TArray<USoundBase*> HitSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Effect")
	TArray<UNiagaraSystem*> ReboundHit;
};