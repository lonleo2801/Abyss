// Copyright (c) 2025 Leon Lee


#include "AbyssCombatComponent_Hero.h"

#include "Characters/AbyssPawn.h"
#include "Characters/Hero/Character/AbyssHeroBase.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/WeaponInstance.h"


UAbyssCombatComponent_Hero::UAbyssCombatComponent_Hero()
{
	SetIsReplicatedByDefault(true);
}

void UAbyssCombatComponent_Hero::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAbyssCombatComponent_Hero, CurrentWeapon);
}

void UAbyssCombatComponent_Hero::EquipWeapon(const FDataRegistryId& InWeaponId)
{
	CurrentWeapon = NewObject<UWeaponInstance>(this);
	CurrentWeapon->Initialize(InWeaponId, GetOwner());
}

UAbilitySystemComponent* UAbyssCombatComponent_Hero::GetOwningAbilitySystemComponent() const
{
	return GetOwningPawn<AAbyssPawn>()->GetAbilitySystemComponent();
}

UAttributeSet* UAbyssCombatComponent_Hero::GetOwningAttributeSet() const
{
	return GetOwningPawn<AAbyssPawn>()->GetAttributeSet();
}


