// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbyssCombatComponentBase.h"
#include "GameplayTagContainer.h"
#include "AbyssCombatComponent_Hero.generated.h"


class UAbyssPlayerAbilityRuntimeData;
struct FDataRegistryId;
class UWeaponInstance;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ABYSS_API UAbyssCombatComponent_Hero : public UAbyssCombatComponentBase
{
	GENERATED_BODY()

public:
	UAbyssCombatComponent_Hero();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	TObjectPtr<UWeaponInstance> CurrentWeapon;

	UFUNCTION(BlueprintCallable)
	void EquipWeapon(const FDataRegistryId& InWeaponId);
	
	virtual UAbilitySystemComponent* GetOwningAbilitySystemComponent() const override;
	virtual UAttributeSet* GetOwningAttributeSet() const override;
};
