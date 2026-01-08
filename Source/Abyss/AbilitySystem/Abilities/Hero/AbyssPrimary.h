// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AbyssGameplayAbility.h"
#include "AbyssPrimary.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssPrimary : public UAbyssGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Abyss|Abilities")
	TArray<AActor*> HitBoxOverlapTest();

	UFUNCTION(BlueprintCallable, Category = "Crash|Abilities")
	void SendHitReactEventToActors(const TArray<AActor*>& ActorsHit);

private:
	void DrawHitBoxOverlapDebugs(const TArray<FOverlapResult>& OverlapResults, const FVector& HitBoxLocation) const;
	
private:

	UPROPERTY(EditDefaultsOnly, Category = "Abyss|Abilities")
	float HitBoxRadius = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Abyss|Abilities")
	float HitBoxForwardOffset = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Abyss|Abilities")
	float HitBoxElevationOffset = 20.0f;
};
