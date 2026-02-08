// Copyright (c) 2025 Leon Lee

#pragma once
#include "AbilitySystem/Abilities/AbyssGameplayAbility.h"
#include "CoreMinimal.h"
#include "AbyssGameplayAbility_Reload.generated.h"
/**
 *
 */
UCLASS()
class ABYSS_API UAbyssGameplayAbility_Reload : public UAbyssGameplayAbility {
  GENERATED_BODY()
public:
  UAbyssGameplayAbility_Reload();
  virtual void
  ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                  const FGameplayAbilityActorInfo *ActorInfo,
                  const FGameplayAbilityActivationInfo ActivationInfo,
                  const FGameplayEventData *TriggerEventData) override;

  UFUNCTION(BlueprintImplementableEvent)
  void BP_PlayMontage(UAnimMontage *Montage);

protected:
  UFUNCTION(BlueprintCallable)
  void OnReloadComplete();

  UFUNCTION(BlueprintCallable)
  void OnReloadCancelled();

  UPROPERTY()
  TObjectPtr<UAnimMontage> ReloadMontage;

  /** 防止 OnReloadComplete 被多次调用的标志 */
  UPROPERTY()
  bool bReloadCompleted = false;
};
