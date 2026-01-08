// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbyssProjectileAbility.h"
#include "AbyssFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssFireBolt : public UAbyssProjectileAbility
{
	GENERATED_BODY()
public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
	
	UFUNCTION(BlueprintCallable, Category = "Abyss|Projectile")
	void SpawnProjectiles(FVector ProjectileTargetLocation= FVector::ZeroVector,FGameplayTag SocketTag = FGameplayTag(), bool bOverridePitch =  false, float PitchOverride = 0.f, AActor* HomingTarget =  nullptr);
protected:

	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float ProjectileSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	int32 MaxNumProjectiles = 5;
	
	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float HomingAccelerationMin = 2600.f;

	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float HomingAccelerationMax = 5200.f;

	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	bool bLaunchHomingProjectiles = true;
};
