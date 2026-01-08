// Copyright (c) 2025 Leon Lee


#include "WeaponInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/AbyssAbilitySystemComponent.h"
#include "Data/AbyssDataRegistryManager.h"
#include "Net/UnrealNetwork.h"

void UWeaponInstance::Initialize(const FDataRegistryId& InWeaponId, AActor* InOwner)
{
	WeaponId = InWeaponId;
	OwnerActor = InOwner;
	LoadWeaponData();
}

void UWeaponInstance::OnRep_WeaponId()
{
	LoadWeaponData();
}

void UWeaponInstance::ActivateWeaponAbility()
{
	if (!OwnerActor) return;

	UAbyssAbilitySystemComponent* AbyssASC = Cast<UAbyssAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor));
	if (!AbyssASC || !CachedWeaponData.AttackAbility) return;

	AbyssASC->TryActivateAbilityByClass(CachedWeaponData.AttackAbility);
	
}

void UWeaponInstance::OnEquipped()
{
}

void UWeaponInstance::OnUnequipped()
{
}

void UWeaponInstance::LoadWeaponData()
{
	if (const FAbyssWeaponDataRow* FoundData = UAbyssDataRegistryManager::FindCachedSync<FAbyssWeaponDataRow>(WeaponId.ItemName, WeaponId.RegistryType))
	{
		CachedWeaponData = *FoundData;
		// 成功加载，可更新外观或动画
	}
}

void UWeaponInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWeaponInstance, WeaponId);
}

