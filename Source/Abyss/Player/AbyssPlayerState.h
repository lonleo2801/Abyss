// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Data/RunTimeDataClass/AbyssPlayerAbilityRuntimeData.h"
#include "GameFramework/PlayerState.h"
#include "AbyssPlayerState.generated.h"

class UAbyssAttributeSet;
/**
 * 
 */
UCLASS()
class ABYSS_API AAbyssPlayerState : public APlayerState,public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAbyssPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UAbyssAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	UAbyssPlayerAbilityRuntimeData* GetRuntimePlayerAbilityData() const {return RuntimePlayerAbilityData;}
	
	void InitRuntimeData(AAbyssHeroBase* Hero,FGameplayTag CharacterTypeTag); 
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Abyss|Abilities")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAbyssAttributeSet> AttributeSet;
	
	UPROPERTY()
	TObjectPtr<UAbyssPlayerAbilityRuntimeData> RuntimePlayerAbilityData;
};
