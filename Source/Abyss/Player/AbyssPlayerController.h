// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AbyssPlayerController.generated.h"

class UAbilitySystemComponent;
class UAbyssViewModel_GameMenuInteraction;
class AAbyssHeroBase;
class UAbyssDamageTextComponent;
class UAbyssAttributeSet;
class UAbyssViewModel_CharacterAttributes;
class UCommonActivatableWidget;
class UAbyssAbilitySystemComponent;
struct FGameplayTag;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UUserWidget;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class AAbyssPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	UAbyssAbilitySystemComponent* GetASC();
	
	/** UI相关 */
	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount,ACharacter* TargetCharacter,bool bBlockHit,bool bCritHit);
	void InitCommonUIRootLayer() const;
	void PushHudToRootLayer() const;
	void InitCharacterAttributesViewModel();
	void BindAttributesChangeDelegateToViewModel();

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Abyss|UI")
	TSoftClassPtr<UCommonActivatableWidget> HUDClass;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Abyss|UI")
	TSoftClassPtr<UCommonActivatableWidget> MainMenuClass;
	
	UPROPERTY(EditDefaultsOnly,Category = "Abyss|UI")
	TSubclassOf<UAbyssDamageTextComponent> DamageTextComponentClass;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAbyssViewModel_CharacterAttributes> CharacterAttributesViewModel;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAbyssViewModel_GameMenuInteraction> GameMenuInteractionViewModel;
protected:
	
	/** Gameplay initialization */
	virtual void BeginPlay() override;
	
	virtual void OnPossess(APawn* InPawn) override;
	
	virtual void OnRep_PlayerState() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

private:
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category = "Abyss|Input|Movement")
	TArray<TObjectPtr<UInputMappingContext>> InputMappingContexts;

	UPROPERTY(EditDefaultsOnly, Category = "Abyss|Input|Movement")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Abyss|Input|Movement")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Abyss|Input|Movement")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Abyss|Input|Abilities")
	TObjectPtr<UInputAction> PrimaryAction;

	UPROPERTY(EditDefaultsOnly, Category = "Abyss|Input|Abilities")
	TObjectPtr<UInputAction> SecondaryAction;

	UPROPERTY(EditDefaultsOnly, Category = "Abyss|Input|Abilities")
	TObjectPtr<UInputAction> TertiaryAction;

private:
	UPROPERTY()
	TWeakObjectPtr<UAbyssAbilitySystemComponent> AbyssAbilitySystemComponent;
	
	void Jump();
	void StopJumping();
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
	void ActivateAbility(const FGameplayTag& AbilityTag) const;
};
