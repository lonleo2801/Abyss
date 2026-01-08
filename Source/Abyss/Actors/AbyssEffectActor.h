// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFramework/Actor.h"
#include "AbyssEffectActor.generated.h"

class UAbilitySystemComponent;

class UGameplayEffect;
class USphereComponent;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};
UENUM(BlueprintType)
enum class EEffectRemovalPolicy : uint8
{
	RemoveOnEndOverlap,
	DoNotRemoval
};

UCLASS()
class ABYSS_API AAbyssEffectActor : public AActor
{
	GENERATED_BODY()

public:
	AAbyssEffectActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor,TSubclassOf<UGameplayEffect> GameplayEffectClass);
	
	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Abyss|Applied Effects")
	bool bDestroyOnEffectApplication=false;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Abyss|Applied Effects")
	bool bApplyEffectToEnemies=false;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Abyss|Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Abyss|Applied Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Abyss|Applied Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Abyss|Applied Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Abyss|Applied Effects")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Abyss|Applied Effects")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Abyss|Applied Effects")
	EEffectRemovalPolicy InfiniteEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Abyss|Applied Effects")
	int32 InfiniteEffectRemovalStack = 1;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Abyss|Applied Effects")
	float ActorLevel=1.f;
	
	//已经应用的有持续时间的buff的handle map
	UPROPERTY()
	TMap<FActiveGameplayEffectHandle,UAbilitySystemComponent*> ActiveDurationEffectHandles;
	
	//已经应用的无限buff的handle map
	UPROPERTY()
	TMap<FActiveGameplayEffectHandle,UAbilitySystemComponent*> ActiveInfiniteEffectHandles;

private:
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
};
