// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "UAbyssDamageGameplayEffectExecCalc.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UUAbyssDamageGameplayEffectExecCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UUAbyssDamageGameplayEffectExecCalc();
	void DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
						 const FGameplayEffectSpec& Spec,
						 FAggregatorEvaluateParameters EvaluationParameters) const;

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
