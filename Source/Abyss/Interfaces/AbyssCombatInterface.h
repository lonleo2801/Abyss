// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "AbyssCombatInterface.generated.h"

class UAbilitySystemComponent;
class UAbyssCombatComponentBase;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnASCRegistered, UAbilitySystemComponent*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, AActor*, DeadActor);
// This class does not need to be modified.
UINTERFACE()
class UAbyssCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 * 
 */
class ABYSS_API IAbyssCombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	virtual UAbyssCombatComponentBase* GetCombatComponent() const=0;
	virtual void Die(const FVector& DeathImpulse)=0;
	virtual bool IsDead() const = 0;
	virtual FOnASCRegistered GetOnAscRegisteredDelegate()const =0;
	virtual FOnDeathSignature GetOnDeathDelegate()const =0;
	virtual FGameplayTag GetCharacterClass() const =0;
	virtual void LevelUp(int32 NewLevel) =0;
	
};
