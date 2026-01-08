// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "AbyssAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	
public:
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
