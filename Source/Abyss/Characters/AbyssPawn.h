// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/AbyssCombatInterface.h"
#include "AbyssPawn.generated.h"

class UAbyssCombatComponent_Hero;
class UGameplayEffect;
class UDebuffNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class ABYSS_API AAbyssPawn : public APawn , public IAbilitySystemInterface, public IAbyssCombatInterface
{
	GENERATED_BODY()

public:
	AAbyssPawn();
	
	//~begin IAbyssCombatInterface
	virtual UAbyssCombatComponentBase* GetCombatComponent()const override;
	virtual void Die(const FVector& DeathImpulse) override;
	virtual bool IsDead() const override {return bDead;}
	virtual FOnASCRegistered GetOnAscRegisteredDelegate()const override;
	virtual FOnDeathSignature GetOnDeathDelegate()const override;
	virtual FGameplayTag GetCharacterClass() const override {return CharacterTypeTag;}
	virtual void LevelUp(int32 NewLevel)  override{}
	//~end IAbyssCombatInterface

	//~begin IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~end IAbilitySystemInterface
	
	UAttributeSet* GetAttributeSet() const;
	
	void InitializeDefaultAttributes() const;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void InitAbilityActorInfo();

	void AddCharacterStartupAbilities() const;
	
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;
	
	FOnASCRegistered OnAscRegistered;
	UPROPERTY()
	FOnDeathSignature OnDeathDelegate;

public:
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDebuffNiagaraComponent> DebuffComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = "Abyss|CharacterType",meta=(Categories="AbyssTags.CharacterClass"))
	FGameplayTag CharacterTypeTag;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Components",meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UAbyssCombatComponent_Hero> HeroCombatComponent;
	
	//~Begin 死亡相关

	UFUNCTION(NetMulticast,Reliable)
	virtual void MulticastHandleDeath(const FVector& DeathImpulse);
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_HandleDeath(const FVector& DeathImpulse);

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Abyss|Combat")
	UNiagaraSystem* BloodEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Abyss|Combat")
	USoundBase* DeathSound;
	
	bool bDead = false;
	//~End 死亡相关
};
