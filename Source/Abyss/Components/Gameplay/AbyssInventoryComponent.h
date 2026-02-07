// Copyright (c) 2025 Leon Lee
#pragma once

#include "Actors/Weapons/WeaponDefinition.h"
#include "Components/AbyssExtensionComponentBase.h"
#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "AbyssInventoryComponent.generated.h"

struct FInputActionValue;

USTRUCT()
struct FAbyssAbilitySetHandle {
  GENERATED_BODY()

  // 把 TArray 包在这里面
  UPROPERTY()
  TArray<FGameplayAbilitySpecHandle> Handles;
};
class AAbyssWeaponBase;
// 委托：当当前武器发生变化时（通知 HUD 更新弹药/准星）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentWeaponChanged,
                                            AAbyssWeaponBase *, NewWeapon);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ABYSS_API UAbyssInventoryComponent : public UAbyssExtensionComponentBase {
  GENERATED_BODY()

public:
  UAbyssInventoryComponent();

  virtual void GetLifetimeReplicatedProps(
      TArray<FLifetimeProperty> &OutLifetimeProps) const override;

  // --- 核心交互 API (由 InteractionComponent 调用) ---

  // 尝试拾取武器 (Server Only)
  UFUNCTION(BlueprintCallable, Server, Reliable)
  void ServerPickupWeapon(AAbyssWeaponBase *WeaponToPickup);

  // 尝试切换/装备武器 (Server Only)
  UFUNCTION(BlueprintCallable, Server, Reliable)
  void ServerEquipWeapon(EWeaponSlot SlotToEquip);

  // 丢弃当前武器 (Server Only)
  UFUNCTION(BlueprintCallable, Server, Reliable)
  void ServerDropWeapon();

  // 主动收起当前武器 (Server Only)
  UFUNCTION(BlueprintCallable, Server, Reliable)
  void ServerHolsterWeapon();

  // 切换到上一把武器 (Server Only)
  UFUNCTION(BlueprintCallable, Server, Reliable)
  void ServerSwapToPreviousWeapon();

  // --- 查询 API ---

  UFUNCTION(BlueprintPure)
  AAbyssWeaponBase *GetCurrentWeapon() const { return CurrentWeapon; }

  UFUNCTION(BlueprintPure)
  AAbyssWeaponBase *GetWeaponInSlot(EWeaponSlot Slot) const;

  // --- 工具函数 (内部) ---

  // 获取指定槽位的武器数组 (引用返回，不可用于 Melee)
  TArray<AAbyssWeaponBase *> &GetWeaponArrayForSlot(EWeaponSlot Slot);
  const TArray<AAbyssWeaponBase *> &
  GetWeaponArrayForSlot(EWeaponSlot Slot) const;

  // 获取槽位中所有武器 (Blueprint API)
  UFUNCTION(BlueprintCallable)
  TArray<AAbyssWeaponBase *> GetAllWeaponsInSlot(EWeaponSlot Slot) const;

  bool IsSlotFull(EWeaponSlot Slot) const;

  AAbyssWeaponBase *GetLastUsedWeaponInSlot(EWeaponSlot Slot) const;

  void UpdateLastUsedIndex(AAbyssWeaponBase *NewWeapon);

  // 从背包数据中移除 (但不处理物理/销毁)
  void RemoveWeaponFromInventory(AAbyssWeaponBase *Weapon);

  // 统一的内部装备逻辑
  void EquipWeaponInternal(AAbyssWeaponBase *NewWeapon);

  // 丢弃指定武器
  void DropSpecificWeapon(AAbyssWeaponBase *Weapon);

  // --- 委托 ---
  UPROPERTY(BlueprintAssignable)
  FOnCurrentWeaponChanged OnCurrentWeaponChanged;

protected:
  virtual void BeginPlay() override;

  static constexpr int32 MaxWeaponsPerSlot = 2;

  // --- 核心数据 (Replicated) ---

  // 长枪槽位 (最多2把)
  UPROPERTY(Replicated)
  TArray<AAbyssWeaponBase *> PrimaryWeapons;

  // 手枪槽位 (最多2把)
  UPROPERTY(Replicated)
  TArray<AAbyssWeaponBase *> SecondaryWeapons;

  // 近战槽位 (仅1把)
  UPROPERTY(Replicated)
  AAbyssWeaponBase *MeleeWeapon;

  // 每个槽位上次使用的武器索引 (用于替换逻辑)
  UPROPERTY()
  int32 LastUsedPrimaryIndex = 0;

  UPROPERTY()
  int32 LastUsedSecondaryIndex = 0;

  // 当前手持的武器
  UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon)
  AAbyssWeaponBase *CurrentWeapon;

  // 上一把武器（用于快速切换/Q键）
  UPROPERTY()
  AAbyssWeaponBase *PreviousWeapon;

  // --- 内部逻辑 ---

  UFUNCTION()
  void OnRep_CurrentWeapon(AAbyssWeaponBase *OldWeapon);

  // GAS: 赋予武器技能
  void GrantWeaponAbilities(AAbyssWeaponBase *Weapon);

  // GAS: 移除武器技能
  void RemoveWeaponAbilities(AAbyssWeaponBase *Weapon);

  // GAS: 服务器端记录 Handle 的 Map (不需要复制，仅服务器权威管理)
  UPROPERTY()
  TMap<AAbyssWeaponBase *, FAbyssAbilitySetHandle> WeaponAbilityHandles;

protected:
  // Input
  UPROPERTY(EditDefaultsOnly, Category = "Abyss|Input")
  class UInputAction *PrimaryWeaponsAction;

  UPROPERTY(EditDefaultsOnly, Category = "Abyss|Input")
  class UInputAction *SecondaryWeaponsAction;

  UPROPERTY(EditDefaultsOnly, Category = "Abyss|Input")
  class UInputAction *MeleeWeaponsAction;

  UPROPERTY(EditDefaultsOnly, Category = "Abyss|Input")
  class UInputAction *PreviousWeaponAction;

  UPROPERTY(EditDefaultsOnly, Category = "Abyss|Input")
  class UInputAction *DropWeaponAction;

  // Input Callbacks
  void OnPrimaryWeaponsAction(const FInputActionValue &Value);
  void OnSecondaryWeaponsAction(const FInputActionValue &Value);
  void OnMeleeWeaponsAction(const FInputActionValue &Value);
  void OnPreviousWeaponAction(const FInputActionValue &Value);
  void OnDropWeaponAction(const FInputActionValue &Value);

public:
  // --- Ammo System ---

  /**
   * Get the amount of reserve ammo for a specific type.
   * @param AmmoType The type of ammo to check.
   * @return The current reserve count.
   */
  UFUNCTION(BlueprintPure, Category = "Abyss|Ammo")
  int32 GetReserveAmmo(EAmmoType AmmoType) const;

  /**
   * Add ammo to the reserve.
   * @param AmmoType The type of ammo to add.
   * @param Amount The amount to add.
   */
  UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Abyss|Ammo")
  void AddReserveAmmo(EAmmoType AmmoType, int32 Amount);

  /**
   * Consume ammo from the reserve.
   * @param AmmoType The type of ammo to use.
   * @param Amount The amount to consume.
   * @return True if successful (enough ammo existed).
   */
  UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Abyss|Ammo")
  bool ConsumeReserveAmmo(EAmmoType AmmoType, int32 Amount);

  /**
   * Helper to determine ammo type from a weapon slot.
   * @param Slot The weapon slot.
   * @return The corresponding default ammo type.
   */
  UFUNCTION(BlueprintPure, Category = "Abyss|Ammo")
  EAmmoType GetAmmoTypeForSlot(EWeaponSlot Slot) const;

protected:
  UPROPERTY(Replicated)
  int32 RifleReserveAmmo = 60; // Default for testing
  
  UPROPERTY(EditDefaultsOnly, Category = "Abyss|Ammo")
  int32 MaxRifleReserveAmmo = 180;

  UPROPERTY(Replicated)
  int32 PistolReserveAmmo = 30; // Default for testing
  
  UPROPERTY(EditDefaultsOnly, Category = "Abyss|Ammo")
  int32 MaxPistolReserveAmmo = 60;
};
