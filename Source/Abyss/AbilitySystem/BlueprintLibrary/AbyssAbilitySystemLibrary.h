// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Data/RunTimeDataClass/AbyssPlayerAbilityRuntimeData.h"
#include "AbyssAbilitySystemLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssAbilitySystemLibrary : public UAbilitySystemBlueprintLibrary
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Abyss|AbyssAbilitySystemLibrary|GameplayMechanics")
	static void GetLiveCharacterWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors,const TArray<AActor*>& ActorsToIgnore, float Radius,const FVector& SphereOrigin);
	
	UFUNCTION(BlueprintPure, Category = "Abyss|AbyssAbilitySystemLibrary|GameplayMechanics")
	static void GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors,TArray<AActor*>& OutClosestTargets,const FVector& Origin);
	
	UFUNCTION(BlueprintPure, Category = "Abyss|AbyssAbilitySystemLibrary|GameplayMechanics")
	static bool IsNotFriend(AActor* FirstActor,AActor* SecondActor);
	
	UFUNCTION(BlueprintCallable, Category = "Abyss|AbyssAbilitySystemLibrary|GameplayMechanics")
	static UAbyssPlayerAbilityRuntimeData* GetPlayerAbilityRuntimeData(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category = "Abyss|AbyssAbilitySystemLibrary|GameplayMechanics")
	static TArray<FRotator> EvenlySpacedRotators(const FVector& Forward,const FVector& Axis,float Spread,int32 NumRotators);
	
	UFUNCTION(BlueprintPure, Category = "Abyss|AbyssAbilitySystemLibrary|GameplayMechanics")
	static TArray<FVector> EvenlyRotatedVectors(const FVector& Forward,const FVector& Axis,float Spread, int32 NumVectors);

};






