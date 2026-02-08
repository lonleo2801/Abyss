// Copyright (c) 2025 Leon Lee

#include "AbilitySystem/Abilities/Weapons/AbyssGameplayAbility_Reload.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Actors/Weapons/AbyssWeaponBase.h"
#include "Characters/AbyssPawn.h"
#include "Components/Gameplay/AbyssInventoryComponent.h"

UAbyssGameplayAbility_Reload::UAbyssGameplayAbility_Reload() {
  NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
  InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UAbyssGameplayAbility_Reload::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData *TriggerEventData) {
  // 重置标志
  bReloadCompleted = false;

  if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) {
    EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    return;
  }
  AAbyssPawn *AvatarPawn = Cast<AAbyssPawn>(GetAvatarActorFromActorInfo());
  UAbyssInventoryComponent *Inventory =
      AvatarPawn ? AvatarPawn->GetInventoryComponent() : nullptr;
  if (!Inventory) {
    EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    return;
  }
  AAbyssWeaponBase *Weapon = Inventory->GetCurrentWeapon();
  if (!Weapon) {
    EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    return;
  }

  // 检查是否需要装填
  if (Weapon->GetCurrentAmmo() >= Weapon->GetMaxClipSize()) {
    // 满弹，不需要装填
    EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    return;
  }

  // 检查备弹
  int32 Reserve = Inventory->GetReserveAmmo(Weapon->GetAmmoType());
  if (Reserve <= 0) {
    // 没备弹
    EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    return;
  }
  // 设置要播放的动画
  ReloadMontage =
      Weapon->WeaponDef ? Weapon->WeaponDef->ReloadMontage : nullptr;

  BP_PlayMontage(ReloadMontage);
}

void UAbyssGameplayAbility_Reload::OnReloadComplete() {
  // 防止多次调用（蓝图可能在客户端和服务器都调用了这个函数）
  if (bReloadCompleted) {
    return;
  }
  bReloadCompleted = true;

  // 真正装填逻辑 - 只在服务器执行
  if (GetOwningActorFromActorInfo()->HasAuthority()) {
    AAbyssPawn *AvatarPawn = Cast<AAbyssPawn>(GetAvatarActorFromActorInfo());
    UAbyssInventoryComponent *Inventory =
        AvatarPawn ? AvatarPawn->GetInventoryComponent() : nullptr;
    if (Inventory == nullptr) {
      EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
                 true, false);
      return;
    }
    AAbyssWeaponBase *Weapon = Inventory->GetCurrentWeapon();

    if (Weapon && Inventory) {
      int32 CurrentClip = Weapon->GetCurrentAmmo();
      int32 MaxClip = Weapon->GetMaxClipSize();
      int32 Needed = MaxClip - CurrentClip;

      EAmmoType AmmoType = Weapon->GetAmmoType();
      int32 Reserve = Inventory->GetReserveAmmo(AmmoType);

      int32 ToReload = FMath::Min(Needed, Reserve);

      if (ToReload > 0 && Inventory->ConsumeReserveAmmo(AmmoType, ToReload)) {
        Weapon->AddAmmo(ToReload);
      }
    }
  }

  EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true,
             false);
}

void UAbyssGameplayAbility_Reload::OnReloadCancelled() {
  EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true,
             false);
}
