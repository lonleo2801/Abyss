// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AbyssWaitCooldownChange.generated.h"



class UAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangeDelegate, float, CooldownRemaining);

/**
 * 
 */
UCLASS(Blueprintable ,meta = (ExposedAsyncProxy = "AsyncTask"))
class ABYSS_API UAbyssWaitCooldownChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeDelegate OnCooldownStart;
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeDelegate OnCooldownEnd;
	
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAbyssWaitCooldownChange* WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCooldownTag);
	
	UFUNCTION(BlueprintCallable)
	void EndTask();
	
protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	FGameplayTag CooldownTag;
	
	void OnCooldownTagChanged(const FGameplayTag InCooldownTag,int32 NewCount);
	
	void OnActiveEffectAdded(UAbilitySystemComponent* TargetAsc,const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle);
};


















