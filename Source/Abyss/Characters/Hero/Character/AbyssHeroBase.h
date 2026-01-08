// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbyssTypes.h"
#include "Characters/AbyssCharacterBase.h"
#include "AbyssHeroBase.generated.h"

class UCommonActivatableWidget;
class UAbyssCombatComponent_Hero;
class UCameraComponent;
class USpringArmComponent;
class UGameplayAbility;


class UMotionWarpingComponent;
class UCharacterTrajectoryComponent;

UCLASS()
class ABYSS_API AAbyssHeroBase : public AAbyssCharacterBase 
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAbyssHeroBase();
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// AbilityCharacterBase
	virtual UAbilitySystemComponent* GetAbilitySystemComponent()const override;
	virtual UAttributeSet* GetAttributeSet()const override;
	virtual void AddCharacterStartupAbilities() const override;
	virtual void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const override;
	virtual void InitializeDefaultAttributes() const override;
	// AbilityCharacterBase
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE USkeletalMeshComponent* GetAnimMesh() const { return AnimMesh; }

	FORCEINLINE UAbyssCombatComponent_Hero* GetHeroCombatComponent() const { return HeroCombatComponent; }
	
	//~begin IAbyssCombatInterface
	virtual UAbyssCombatComponentBase* GetCombatComponent()const override;
	virtual void LevelUp(int32 NewLevel) override;
	//~end IAbyssCombatInterface
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
public:
	bool IsAttack();
	bool IsCrouch();
	bool IsDie();
	
	ECharacterGaitType GetGaitType();
	bool IsJustLanded();
	FVector GetLandVelocity();
protected:
	virtual void InitAbilityActorInfo() override;
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> AnimMesh;

private:
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Components",meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UAbyssCombatComponent_Hero> HeroCombatComponent;


};
