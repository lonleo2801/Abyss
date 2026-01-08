// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbyssAbilitySystemComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer&, AttributesTagContainer);



UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ABYSS_API UAbyssAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void AbilityActorInfoSet();

	// Effect 应用时，用来广播Effect添加的Tag
	UPROPERTY(BlueprintAssignable)
	FEffectAssetTags EffectAssetTags;

	void AddCharacterStartupAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	
	UFUNCTION(BlueprintCallable, Category = "Abyss|Abilities")
	void SetAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level);

	UFUNCTION(BlueprintCallable, Category = "Abyss|Abilities")
	void AddToAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level = 1);

protected:
	
	virtual void OnRep_ActivateAbilities() override;

private:

	UFUNCTION(client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent,const FGameplayEffectSpec& EffectSpec,FActiveGameplayEffectHandle ActiveEffectHandle);
};
