// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "AbyssWaitGameplayEvent.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssWaitGameplayEvent : public UAbilityAsync_WaitGameplayEvent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Async", meta = (DefaultToSelf = "TargetActor", BlueprintInternalUseOnly = "TRUE"))
	static UAbyssWaitGameplayEvent* WaitGameplayEventToActorProxy(AActor* TargetActor, UPARAM(meta=(GameplayTagFilter="GameplayEventTagsCategory")) FGameplayTag EventTag, bool OnlyTriggerOnce = false, bool OnlyMatchExact = true);

	
	void StartActivation();
};
