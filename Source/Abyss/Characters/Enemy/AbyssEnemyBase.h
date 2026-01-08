// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Characters/AbyssCharacterBase.h"
#include "Interfaces/AbyssEnemyInterface.h"
#include "AbyssEnemyBase.generated.h"

class UEnemyHealthWidgetComponent;
class UAbyssViewModel_EnemyAttributes;
class UAbyssCombatComponent_Enemy;
class UAttributeSet;

UCLASS()
class ABYSS_API AAbyssEnemyBase : public AAbyssCharacterBase ,public IAbyssEnemyInterface
{
	GENERATED_BODY()

public:
	AAbyssEnemyBase();

	FORCEINLINE UAbyssCombatComponent_Enemy* GetEnemyCombatComponent() const { return EnemyCombatComponent; }
	//~begin IAbyssCombatInterface
	virtual UAbyssCombatComponentBase* GetCombatComponent()const override;
	//~end IAbyssCombatInterface
	
	// AbilityCharacterBase
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet()const override;
	virtual void AddCharacterStartupAbilities() const override;
	virtual void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const override;
	virtual void InitializeDefaultAttributes() const override;
	virtual void Die(const FVector& DeathImpulse) override;
	// AbilityCharacterBase

	// ~begin IAbyssEnemyInterface
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	//~end IAbyssEnemyInterface
	
	UAbyssViewModel_EnemyAttributes* GetEnemyAttributesViewModel() const {return EnemyAttributesViewModel;}

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Combat")
	float LifeSpan = 5.f;
private:
	void InitAttributesViewModel();
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Components",meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UAbyssCombatComponent_Enemy> EnemyCombatComponent;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Components",meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UEnemyHealthWidgetComponent> HealthWidgetComponent;
	
	UPROPERTY(BlueprintReadOnly,Category = "Abyss|Enemy|UI",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbyssViewModel_EnemyAttributes> EnemyAttributesViewModel;
	
};
