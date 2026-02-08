// Copyright (c) 2025 Leon Lee

#include "AbyssWeaponBase.h"
#include "Components/Gameplay/AbyssInventoryComponent.h"
#include "Net/UnrealNetwork.h"

AAbyssWeaponBase::AAbyssWeaponBase() { CurrentState = EWeaponState::OnGround; }

void AAbyssWeaponBase::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty> &OutLifetimeProps) const {
  Super::GetLifetimeReplicatedProps(OutLifetimeProps);
  DOREPLIFETIME(AAbyssWeaponBase, CurrentState);
  DOREPLIFETIME(AAbyssWeaponBase, CurrentAmmoInClip);
}

void AAbyssWeaponBase::BeginPlay() {
  Super::BeginPlay();

  // 游戏开始时，确保物理和挂载状态正确
  SetReplicateMovement(true); // 掉在地上需要同步物理位置
  // 如果是编辑器里直接拖到场景里的，它会保持 OnGround
  // 如果是生成出来直接给角色的，SetWeaponState 会被调用
  UpdateAttachmentLogic();

  // Initialize Ammo
  if (HasAuthority() && WeaponDef) {
    CurrentAmmoInClip = WeaponDef->DefaultClipSize;
  }
}

// ============================================================================
// 状态管理 (State Management)
// ============================================================================

void AAbyssWeaponBase::SetWeaponState(EWeaponState NewState) {
  if (HasAuthority()) {
    CurrentState = NewState;

    // 关键：Server 改变变量不会触发自己的 OnRep，必须手动调用
    OnRep_CurrentState();

    // 优化：状态改变通常需要立即同步
    ForceNetUpdate();
  }
}

void AAbyssWeaponBase::OnRep_CurrentState() {
  // Client 和 Server 都会运行这里的逻辑
  UpdateAttachmentLogic();
}

void AAbyssWeaponBase::UpdateAttachmentLogic() {
  // 检查数据资产有效性
  if (!WeaponDef) {
    UE_LOG(LogTemp, Warning, TEXT("Weapon %s matches no WeaponDefinition!"),
           *GetName());
    return;
  }

  // -----------------------------------------------------------
  // 状态 A: 在地上 (On Ground)
  // -----------------------------------------------------------
  if (CurrentState == EWeaponState::OnGround) {
    // 1. 先 Detach
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    // 2. 开启物理和碰撞
    OpenPickupMeshSimulatePhysics(true);

    return;
  }

  // -----------------------------------------------------------
  // 状态 B & C: 装备中 (Equipped / Holstered)
  // -----------------------------------------------------------

  // 1. 关闭物理 (必须先关物理，才能 Attach)
  OpenPickupMeshSimulatePhysics(false);

  // 2. 寻找 Owner 的 Mesh
  // 优化：支持 Pawn，不仅仅是 Character
  AActor *OwnerActor = GetOwner();
  if (!OwnerActor) {
    return;
  }

  USkeletalMeshComponent *TargetMesh =
      OwnerActor->FindComponentByClass<USkeletalMeshComponent>();

  if (TargetMesh) {
    FName TargetSocket = NAME_None;

    if (CurrentState == EWeaponState::Equipped) {
      TargetSocket = WeaponDef->EquippedSocketName; // 例如 "Hand_R"
    } else if (CurrentState == EWeaponState::Holstered) {
      TargetSocket = WeaponDef->HolsteredSocketName; // 例如 "Holster_Back"
    }

    if (!TargetSocket.IsNone()) {
      AttachToComponent(
          TargetMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale,
          TargetSocket);
    }
  } else {
    UE_LOG(LogTemp, Warning,
           TEXT("Owner %s has no SkeletalMesh to attach weapon!"),
           *OwnerActor->GetName());
  }
}

bool AAbyssWeaponBase::TriggerInteraction_Implementation(APawn *Interactor) {
  // 安全检查：如果枪已经被别人捡了，或者正在被装备，不允许交互
  if (CurrentState != EWeaponState::OnGround) {
    return false;
  }

  // 找到交互者的背包组件
  if (UAbyssInventoryComponent *Inventory =
          Interactor->FindComponentByClass<UAbyssInventoryComponent>()) {
    // 发送拾取请求
    Inventory->ServerPickupWeapon(this);
    return true;
  }

  return false;
}

// ============================================================================
// Ammo System
// ============================================================================

void AAbyssWeaponBase::OnRep_CurrentAmmoInClip(int32 NewAmmo) {
  if (OnClipAmmoChanged.IsBound()) {
    OnClipAmmoChanged.Broadcast(CurrentAmmoInClip);
  }
}

int32 AAbyssWeaponBase::GetMaxClipSize() const {
  if (WeaponDef) {
    return WeaponDef->DefaultClipSize;
  }
  return 0;
}

void AAbyssWeaponBase::ConsumeAmmo(int32 Amount) {
  if (Amount <= 0) {
    return;
  }

  // Ensure we are on server
  if (HasAuthority()) {
    CurrentAmmoInClip = FMath::Max(0, CurrentAmmoInClip - Amount);

    // 服务器修改 Replicated 变量后不会自动触发自己的 OnRep
    // 需要手动调用以触发本地广播
    OnRep_CurrentAmmoInClip(CurrentAmmoInClip);
  }
}

void AAbyssWeaponBase::AddAmmo(int32 Amount) {
  if (Amount <= 0) {
    return;
  }

  if (HasAuthority()) {
    CurrentAmmoInClip =
        FMath::Min(GetMaxClipSize(), CurrentAmmoInClip + Amount);

    // 服务器修改 Replicated 变量后不会自动触发自己的 OnRep
    // 需要手动调用以触发本地广播
    OnRep_CurrentAmmoInClip(CurrentAmmoInClip);
    
  }
}

EAmmoType AAbyssWeaponBase::GetAmmoType() const {
  if (!WeaponDef) {
    return EAmmoType::None;
  }

  // Temporary Logic: Map SlotType directly to AmmoType
  if (WeaponDef->SlotType == EWeaponSlot::Primary) {
    return EAmmoType::Rifle;
  }
  if (WeaponDef->SlotType == EWeaponSlot::Secondary) {
    return EAmmoType::Pistol;
  }

  return EAmmoType::None;
}
