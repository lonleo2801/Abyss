// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Actors/Weapons/WeaponDefinition.h"
#include "Components/AbyssExtensionComponentBase.h"
#include "GameplayAbilitySpec.h"
#include "AbyssInventoryComponent.generated.h"

USTRUCT()
struct FAbyssAbilitySetHandle
{
	GENERATED_BODY()

	// 把 TArray 包在这里面
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> Handles;
};
class AAbyssWeaponBase;
// 委托：当当前武器发生变化时（通知 HUD 更新弹药/准星）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentWeaponChanged, AAbyssWeaponBase*, NewWeapon);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ABYSS_API UAbyssInventoryComponent : public UAbyssExtensionComponentBase
{
	GENERATED_BODY()

public:
	UAbyssInventoryComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// --- 核心交互 API (由 InteractionComponent 调用) ---
    
	// 尝试拾取武器 (Server Only)
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerPickupWeapon(AAbyssWeaponBase* WeaponToPickup);

	// 尝试切换/装备武器 (Server Only)
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerEquipWeapon(EWeaponSlot SlotToEquip);

	// 丢弃当前武器 (Server Only)
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerDropWeapon();

	// --- 查询 API ---
	
	UFUNCTION(BlueprintPure)
	AAbyssWeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }

	UFUNCTION(BlueprintPure)
	AAbyssWeaponBase* GetWeaponInSlot(EWeaponSlot Slot) const;

	UFUNCTION(BlueprintPure)
	EWeaponSlot GetCurrentSlotType() const { return CurrentSlotType; }
	
	// --- 委托 ---
	UPROPERTY(BlueprintAssignable)
	FOnCurrentWeaponChanged OnCurrentWeaponChanged;

protected:
	virtual void BeginPlay() override;

	// --- 核心数据 (Replicated) ---
	
	// 当前激活的槽位 (Replicated) -> 动画蓝图可以用这个
	UPROPERTY(Replicated)
	EWeaponSlot CurrentSlotType;

	// 背包列表：存储所有持有的武器
	UPROPERTY(Replicated)
	TArray<AAbyssWeaponBase*> InventoryList;

	// 当前手持的武器
	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon)
	AAbyssWeaponBase* CurrentWeapon;

	// 上一把武器（用于快速切换/Q键）
	UPROPERTY()
	AAbyssWeaponBase* PreviousWeapon;

	// --- 内部逻辑 ---

	UFUNCTION()
	void OnRep_CurrentWeapon(AAbyssWeaponBase* OldWeapon);

	// GAS: 赋予武器技能
	void GrantWeaponAbilities(AAbyssWeaponBase* Weapon);
    
	// GAS: 移除武器技能
	void RemoveWeaponAbilities(AAbyssWeaponBase* Weapon);

	// GAS: 服务器端记录 Handle 的 Map (不需要复制，仅服务器权威管理)
	UPROPERTY() 
	TMap<AAbyssWeaponBase*, FAbyssAbilitySetHandle> WeaponAbilityHandles;
};
