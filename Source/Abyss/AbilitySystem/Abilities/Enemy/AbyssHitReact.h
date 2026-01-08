// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AbyssGameplayAbility.h"
#include "AbyssHitReact.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssHitReact : public UAbyssGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Abyss|Abilities")
	void CacheHitDirectionVectors(AActor* Instigator);

	UPROPERTY(BlueprintReadOnly, Category = "Abyss|Abilities")
	FVector AvatarForward;

	UPROPERTY(BlueprintReadOnly, Category = "Abyss|Abilities")
	FVector ToInstigator;
};
