// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "Interfaces/AbyssCombatInterface.h"
#include "AbyssCharacterBase.generated.h"

class UNiagaraSystem;
class UDebuffNiagaraComponent;
class UAttributeSet;
class UAbyssAttributeSet;
class UMotionWarpingComponent;
class UAbyssCombatComponentBase;
struct FOnAttributeChangeData;
class UGameplayEffect;
class UGameplayAbility;

namespace AbyssCharacterTags
{
	extern ABYSS_API const FName Hero;
	extern ABYSS_API const FName AnimRedirect;
	extern ABYSS_API const FName Enemy;
}



UCLASS()
class ABYSS_API AAbyssCharacterBase : public ACharacter, public IAbilitySystemInterface , public IAbyssCombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAbyssCharacterBase();
	
	//~begin IAbyssCombatInterface
	virtual UAbyssCombatComponentBase* GetCombatComponent()const override;
	virtual void Die(const FVector& DeathImpulse) override;
	virtual bool IsDead() const override {return bDead;}
	virtual FGameplayTag GetCharacterClass() const override {return CharacterTypeTag;}
	virtual void LevelUp(int32 NewLevel)  override{}
	//~end IAbyssCombatInterface

	//~begin IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~end IAbilitySystemInterface
	virtual UAttributeSet* GetAttributeSet() const;
	
	virtual FOnASCRegistered GetOnAscRegisteredDelegate()const override;
	virtual FOnDeathSignature GetOnDeathDelegate()const override;
	
	UFUNCTION(BlueprintCallable,Category = "Abyss|AI")
	void RotateToTarget(AActor* RotateTarget);
	
	virtual void InitializeDefaultAttributes() const;

protected:
	virtual void InitAbilityActorInfo();

	virtual void AddCharacterStartupAbilities() const;
	
	virtual void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;
	
	FOnASCRegistered OnAscRegistered;
	UPROPERTY()
	FOnDeathSignature OnDeathDelegate;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDebuffNiagaraComponent> BurnDebuffComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = "Abyss|CharacterType",meta=(Categories="AbyssTags.CharacterClass"))
	FGameplayTag CharacterTypeTag;

public:
	//~Begin 死亡相关
	void Dissolve();

	//TODO: 完善死亡溶解
	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);
	
	UFUNCTION(NetMulticast,Reliable)
	virtual void MulticastHandleDeath(const FVector& DeathImpulse);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Abyss|Combat")
	UNiagaraSystem* BloodEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Abyss|Combat")
	USoundBase* DeathSound;
	
	bool bDead = false;
	//~End 死亡相关
};
