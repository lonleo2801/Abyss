// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "DataRegistryId.h"
#include "Data/DataTables/AbyssWeaponDataRow.h"
#include "UObject/Object.h"
#include "WeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
/*运行时逻辑对象（由角色持有），不在世界中存在。*/
class ABYSS_API UWeaponInstance : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(const FDataRegistryId& InWeaponId, AActor* InOwner);

	UPROPERTY(ReplicatedUsing=OnRep_WeaponId)
	FDataRegistryId WeaponId;

	UPROPERTY(Transient)
	FAbyssWeaponDataRow CachedWeaponData;

	UFUNCTION()
	void OnRep_WeaponId();

	// 触发攻击（通过GAS）
	UFUNCTION(BlueprintCallable)
	void ActivateWeaponAbility();


	void OnEquipped();
	void OnUnequipped();

protected:
	UPROPERTY()
	TObjectPtr<AActor> OwnerActor;

	void LoadWeaponData();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
