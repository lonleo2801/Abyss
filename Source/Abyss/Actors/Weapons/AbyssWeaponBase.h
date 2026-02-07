// Copyright (c) 2025 Leon Lee

#pragma once
#include "WeaponDefinition.h"
#include "Actors/AbyssPickupActor.h"
#include "CoreMinimal.h"
#include "AbyssWeaponBase.generated.h"

class UWeaponDefinition;
// 定义武器状态
UENUM(BlueprintType)
enum class EWeaponState : uint8 {
  OnGround UMETA(DisplayName = "On Ground (Physics)"),
  Equipped UMETA(DisplayName = "Equipped (In Hand)"),
  Holstered UMETA(DisplayName = "Holstered (On Back/Hip)")
};
UCLASS()
class ABYSS_API AAbyssWeaponBase : public AAbyssPickupActor {
  GENERATED_BODY()

public:
  AAbyssWeaponBase();

  virtual void GetLifetimeReplicatedProps(
      TArray<FLifetimeProperty> &OutLifetimeProps) const override;

  // --- 数据 ---
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abyss|Setup")
  UWeaponDefinition *WeaponDef;

  // --- 状态管理 (核心) ---
  // 服务器调用此函数来改变状态
  UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Abyss|State")
  void SetWeaponState(EWeaponState NewState);

  // 获取当前状态
  UFUNCTION(BlueprintPure, Category = "Abyss|State")
  EWeaponState GetWeaponState() const { return CurrentState; }

  // --- Ammo System ---
  UPROPERTY(Replicated, VisibleAnywhere,BlueprintReadOnly, Category = "Abyss|State")
  int32 CurrentAmmoInClip;
  /**
   * Get current ammo in clip.
   * @return Current ammo count.
   */
  UFUNCTION(BlueprintPure, Category = "Abyss|Ammo")
  int32 GetCurrentAmmo() const { return CurrentAmmoInClip; }

  /**
   * Get max clip size from definition.
   * @return Max ammo per clip.
   */
  UFUNCTION(BlueprintPure, Category = "Abyss|Ammo")
  int32 GetMaxClipSize() const;

  /**
   * Consume ammo from the clip.
   * @param Amount Amount to consume.
   */
  UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Abyss|Ammo")
  void ConsumeAmmo(int32 Amount);

  /**
   * Add ammo to the clip.
   * @param Amount Amount to add.
   */
  UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Abyss|Ammo")
  void AddAmmo(int32 Amount);

  /**
   * Get the ammo type of this weapon.
   * @return The ammo type.
   */
  UFUNCTION(BlueprintPure, Category = "Abyss|Ammo")
  EAmmoType GetAmmoType() const;

  // --- 接口实现 ---
  virtual bool TriggerInteraction_Implementation(APawn *Interactor) override;

protected:
  virtual void BeginPlay() override;

  // 当前状态 (Replicated)
  UPROPERTY(ReplicatedUsing = OnRep_CurrentState, VisibleAnywhere,
            BlueprintReadOnly, Category = "Abyss|State")
  EWeaponState CurrentState;

  // 状态回调：处理视觉、挂载、物理开关
  UFUNCTION()
  void OnRep_CurrentState();

  // 真正的逻辑执行函数
  void UpdateAttachmentLogic();
};
