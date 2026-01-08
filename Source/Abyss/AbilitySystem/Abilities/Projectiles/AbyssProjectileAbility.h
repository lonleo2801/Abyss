// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AbyssDamageAbility.h"
#include "AbyssProjectileAbility.generated.h"

class AAbyssProjectile;
/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssProjectileAbility : public UAbyssDamageAbility
{
	GENERATED_BODY()
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category = "Abyss|Projectile")
	void SpawnProjectile(FVector ProjectileTargetLocation = FVector::ZeroVector,FGameplayTag SocketTag = FGameplayTag(),bool bOverridePitch=false,float PitchOverride = 0.f);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AAbyssProjectile> ProjectileClass;
	
	UPROPERTY(EditDefaultsOnly)
	int32 NumProjectiles = 5;
};
