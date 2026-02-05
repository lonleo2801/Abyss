// Copyright (c) 2025 Leon Lee

#include "AbyssInventoryComponent.h"

#include "AbilitySystemComponent.h"

#include "Actors/Weapons/AbyssWeaponBase.h"
#include "Net/UnrealNetwork.h"

UAbyssInventoryComponent::UAbyssInventoryComponent() {
  PrimaryComponentTick.bCanEverTick = false;
  SetIsReplicatedByDefault(true);
}

void UAbyssInventoryComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty> &OutLifetimeProps) const {
  Super::GetLifetimeReplicatedProps(OutLifetimeProps);

  DOREPLIFETIME(UAbyssInventoryComponent, PrimaryWeapons);
  DOREPLIFETIME(UAbyssInventoryComponent, SecondaryWeapons);
  DOREPLIFETIME(UAbyssInventoryComponent, MeleeWeapon);

  DOREPLIFETIME(UAbyssInventoryComponent, CurrentWeapon);
}

void UAbyssInventoryComponent::BeginPlay() { Super::BeginPlay(); }

// ============================================================================
// 拾取与自动装备逻辑 (Server Authority)
// ============================================================================
void UAbyssInventoryComponent::ServerPickupWeapon_Implementation(
    AAbyssWeaponBase *WeaponToPickup) {
  if (!WeaponToPickup || !GetOwner() || !WeaponToPickup->WeaponDef)
    return;

  EWeaponSlot TargetSlot = WeaponToPickup->WeaponDef->SlotType;

  // 1. 检查槽位状态
  if (IsSlotFull(TargetSlot)) {
    // 槽位已满，找到要替换的目标
    AAbyssWeaponBase *WeaponToReplace = GetLastUsedWeaponInSlot(TargetSlot);

    // 如果找不到上次用的(异常情况)，也得找一个丢掉
    if (!WeaponToReplace) {
      // 简单策略：如果是近战直接丢MeleeWeapon，如果是枪械丢数组第一个
      if (TargetSlot == EWeaponSlot::Melee)
        WeaponToReplace = MeleeWeapon;
      else {
        TArray<AAbyssWeaponBase *> &Arr = GetWeaponArrayForSlot(TargetSlot);
        if (Arr.Num() > 0)
          WeaponToReplace = Arr[0];
      }
    }

    if (WeaponToReplace) {
      DropSpecificWeapon(WeaponToReplace);
    }
  }

  // 2. 正式接收新武器
  WeaponToPickup->SetOwner(GetOwner());

  if (TargetSlot == EWeaponSlot::Melee) {
    MeleeWeapon = WeaponToPickup;
  } else {
    GetWeaponArrayForSlot(TargetSlot).Add(WeaponToPickup);
  }

  // 3. 直接装备到手上
  // 注意：EquipInternal 会处理 GrantAbilities
  EquipWeaponInternal(WeaponToPickup);
}

// ============================================================================
// 装备/切换逻辑 (Server Authority)
// ============================================================================
void UAbyssInventoryComponent::ServerEquipWeapon_Implementation(
    EWeaponSlot SlotToEquip) {
  // 如果是切换到当前槽位，可能是想切该槽位的另一把枪 (Toggle)
  // 使用 CurrentWeapon 判断当前槽位
  bool bIsCurrentSlot = (CurrentWeapon && CurrentWeapon->WeaponDef &&
                         CurrentWeapon->WeaponDef->SlotType == SlotToEquip);

  if (bIsCurrentSlot && SlotToEquip != EWeaponSlot::Melee) {
    // Toggle Logic for guns
    TArray<AAbyssWeaponBase *> &Arr = GetWeaponArrayForSlot(SlotToEquip);
    if (Arr.Num() > 1) {
      // 找到当前这把的索引，切到另一个
      // 简单逻辑：如果 Current 是 Index 0，就切 1，反之亦然
      // 但我们需要更稳健的逻辑，直接找 "不等于 CurrentWeapon 的那一把"
      for (AAbyssWeaponBase *W : Arr) {
        if (W != CurrentWeapon) {
          EquipWeaponInternal(W);
          return;
        }
      }
    }
    return; // 只有一把或者没找到，不做动作
  }

  // 切换到新槽位
  // 默认切出该槽位 "上次使用" 的那一把，如果没有记录，则切出第一把
  AAbyssWeaponBase *WeaponToEquip = GetLastUsedWeaponInSlot(SlotToEquip);

  // 如果没有"上次使用"记录，尝试获取任意一把
  if (!WeaponToEquip) {
    if (SlotToEquip == EWeaponSlot::Melee) {
      WeaponToEquip = MeleeWeapon;
    } else {
      TArray<AAbyssWeaponBase *> &Arr = GetWeaponArrayForSlot(SlotToEquip);
      if (Arr.Num() > 0)
        WeaponToEquip = Arr[0];
    }
  }

  if (WeaponToEquip) {
    EquipWeaponInternal(WeaponToEquip);
  }
}

void UAbyssInventoryComponent::EquipWeaponInternal(
    AAbyssWeaponBase *NewWeapon) {
  if (!NewWeapon || NewWeapon == CurrentWeapon)
    return;

  AAbyssWeaponBase *OldWeapon = CurrentWeapon;

  // 1. 收起旧武器
  if (OldWeapon) {
    RemoveWeaponAbilities(OldWeapon);
    OldWeapon->SetWeaponState(EWeaponState::Holstered);

    // 更新旧武器槽位的 LastUsedIndex (如果是枪械)
    UpdateLastUsedIndex(OldWeapon);
  }

  // 2. 拿出新武器
  PreviousWeapon = OldWeapon;
  CurrentWeapon = NewWeapon;

  // 更新新武器槽位的 LastUsedIndex (确保当前拿的这把被标记为 LastUsed)
  UpdateLastUsedIndex(CurrentWeapon);

  GrantWeaponAbilities(CurrentWeapon);
  CurrentWeapon->SetWeaponState(EWeaponState::Equipped);

  // 3. 通知客户端
  OnRep_CurrentWeapon(OldWeapon);
}

// ============================================================================
// 收起逻辑 (Server Authority)
// ============================================================================
void UAbyssInventoryComponent::ServerHolsterWeapon_Implementation() {
  if (!CurrentWeapon)
    return;

  AAbyssWeaponBase *WeaponToHolster = CurrentWeapon;

  // 1. 移除能力
  RemoveWeaponAbilities(WeaponToHolster);

  // 2. 也是 Holstered 状态
  WeaponToHolster->SetWeaponState(EWeaponState::Holstered);

  // 更新 LastUsedIndex，确保下次能切回来
  UpdateLastUsedIndex(WeaponToHolster);

  // 3. 记录为 Previous，方便快速切回
  PreviousWeapon = WeaponToHolster;

  // 4. 清空当期
  CurrentWeapon = nullptr;

  // 5. 通知
  OnRep_CurrentWeapon(WeaponToHolster);
}

// ============================================================================
// 切换上一个武器 (Server Authority)
// ============================================================================
void UAbyssInventoryComponent::ServerSwapToPreviousWeapon_Implementation() {
  // 如果 PreviousWeapon 存在且不是当前武器，则切换
  // PreviousWeapon 在 EquipWeaponInternal 和 DropSpecificWeapon 中维护，
  // 保证了如果刚才的枪被丢了，这里就是一个空指针，不会切换
  if (PreviousWeapon && PreviousWeapon != CurrentWeapon) {
    EquipWeaponInternal(PreviousWeapon);
  }
}

// ============================================================================
// 丢弃逻辑 (Server Authority)
// ============================================================================

void UAbyssInventoryComponent::ServerDropWeapon_Implementation() {
  if (!CurrentWeapon)
    return;
  DropSpecificWeapon(CurrentWeapon);
}

void UAbyssInventoryComponent::DropSpecificWeapon(
    AAbyssWeaponBase *WeaponToDrop) {
  if (!WeaponToDrop)
    return;

  bool bIsCurrent = (WeaponToDrop == CurrentWeapon);

  // 1. 如果是当前武器，先移除能力
  // 如果是后台武器，原本就没有能力 (因为我们改成了 Equip 时才 Grant)
  if (bIsCurrent) {
    RemoveWeaponAbilities(WeaponToDrop);
  }

  // 2. 从数据结构移除
  RemoveWeaponFromInventory(WeaponToDrop);

  // 3. 物理丢弃处理
  WeaponToDrop->SetWeaponState(EWeaponState::OnGround);
  WeaponToDrop->SetOwner(nullptr);

  if (UPrimitiveComponent *Root =
          Cast<UPrimitiveComponent>(WeaponToDrop->GetRootComponent())) {
    FVector ThrowDir =
        GetOwner()->GetActorForwardVector() + FVector(0, 0, 0.2f);
    Root->AddImpulse(ThrowDir * 200.f, NAME_None, true);
  }

  // 4. 如果是当前武器，更新引用
  if (bIsCurrent) {
    if (PreviousWeapon == WeaponToDrop)
      PreviousWeapon = nullptr;
    CurrentWeapon = nullptr;

    // Server Context Update
    OnCurrentWeaponChanged.Broadcast(nullptr);

    // 丢弃后不自动切换
  } else {
    // 如果丢的是 PreviousWeapon，也要清空
    if (PreviousWeapon == WeaponToDrop)
      PreviousWeapon = nullptr;
  }
}

// ============================================================================
// 工具函数实现
// ============================================================================

TArray<AAbyssWeaponBase *> &
UAbyssInventoryComponent::GetWeaponArrayForSlot(EWeaponSlot Slot) {
  // 注意：不要对 Melee 调用此函数
  if (Slot == EWeaponSlot::Secondary)
    return SecondaryWeapons;
  return PrimaryWeapons;
}

const TArray<AAbyssWeaponBase *> &
UAbyssInventoryComponent::GetWeaponArrayForSlot(EWeaponSlot Slot) const {
  if (Slot == EWeaponSlot::Secondary)
    return SecondaryWeapons;
  return PrimaryWeapons;
}

TArray<AAbyssWeaponBase *>
UAbyssInventoryComponent::GetAllWeaponsInSlot(EWeaponSlot Slot) const {
  if (Slot == EWeaponSlot::Melee) {
    TArray<AAbyssWeaponBase *> Arr;
    if (MeleeWeapon)
      Arr.Add(MeleeWeapon);
    return Arr;
  }
  return GetWeaponArrayForSlot(Slot);
}

bool UAbyssInventoryComponent::IsSlotFull(EWeaponSlot Slot) const {
  if (Slot == EWeaponSlot::Melee)
    return MeleeWeapon != nullptr;

  // 假设数组里已经是所有该槽位的武器了
  return GetWeaponArrayForSlot(Slot).Num() >= MaxWeaponsPerSlot;
}

AAbyssWeaponBase *
UAbyssInventoryComponent::GetLastUsedWeaponInSlot(EWeaponSlot Slot) const {
  if (Slot == EWeaponSlot::Melee)
    return MeleeWeapon;

  const TArray<AAbyssWeaponBase *> &Arr = GetWeaponArrayForSlot(Slot);
  if (Arr.Num() == 0)
    return nullptr;

  int32 Index = (Slot == EWeaponSlot::Primary) ? LastUsedPrimaryIndex
                                               : LastUsedSecondaryIndex;

  if (Arr.IsValidIndex(Index)) {
    return Arr[Index];
  }

  // 如果索引无效(比如刚丢了一把)，返回第一把
  return Arr[0];
}

void UAbyssInventoryComponent::UpdateLastUsedIndex(AAbyssWeaponBase *Weapon) {
  if (!Weapon || !Weapon->WeaponDef)
    return;
  EWeaponSlot Slot = Weapon->WeaponDef->SlotType;
  if (Slot == EWeaponSlot::Melee)
    return;

  TArray<AAbyssWeaponBase *> &Arr = GetWeaponArrayForSlot(Slot);
  int32 FoundIdx = Arr.Find(Weapon);

  if (FoundIdx != INDEX_NONE) {
    if (Slot == EWeaponSlot::Primary)
      LastUsedPrimaryIndex = FoundIdx;
    else
      LastUsedSecondaryIndex = FoundIdx;
  }
}

void UAbyssInventoryComponent::RemoveWeaponFromInventory(
    AAbyssWeaponBase *Weapon) {
  if (!Weapon || !Weapon->WeaponDef)
    return;
  EWeaponSlot Slot = Weapon->WeaponDef->SlotType;

  if (Slot == EWeaponSlot::Melee) {
    if (MeleeWeapon == Weapon)
      MeleeWeapon = nullptr;
  } else {
    TArray<AAbyssWeaponBase *> &Arr = GetWeaponArrayForSlot(Slot);
    Arr.Remove(Weapon);

    // 移除后修正索引，避免越界
    int32 &IndexRef = (Slot == EWeaponSlot::Primary) ? LastUsedPrimaryIndex
                                                     : LastUsedSecondaryIndex;
    if (IndexRef >= Arr.Num())
      IndexRef = FMath::Max(0, Arr.Num() - 1);
  }
}

// GetWeaponInSlot 保留作为兼容性接口，虽然现在其实可以不用它了
AAbyssWeaponBase *
UAbyssInventoryComponent::GetWeaponInSlot(EWeaponSlot Slot) const {
  return GetLastUsedWeaponInSlot(Slot);
}

void UAbyssInventoryComponent::OnRep_CurrentWeapon(
    AAbyssWeaponBase *OldWeapon) {
  // 注意：AWeaponBase::OnRep_State 已经处理了 Attach Mesh 到 Socket 的逻辑
  // 这里主要处理 "角色" 层面的逻辑，比如 UI 更新、动画状态机变量设置

  // TODO:  广播给 HUD
  OnCurrentWeaponChanged.Broadcast(CurrentWeapon);

  // 如果你有动画层 (AnimInstance)，这里是更新 "IsRifle?", "IsPistol?"
  // 变量的好地方
  if (ACharacter *Character = Cast<ACharacter>(GetOwner())) {
    // 示例：更新 AnimInstance
    // UMyAnimInstance* AnimInst =
    // Cast<UMyAnimInstance>(Character->GetMesh()->GetAnimInstance()); if
    // (AnimInst) AnimInst->SetCurrentWeapon(CurrentWeapon);
  }
}

void UAbyssInventoryComponent::GrantWeaponAbilities(AAbyssWeaponBase *Weapon) {
  if (!Weapon || !Weapon->WeaponDef || !GetOwner())
    return;

  UAbilitySystemComponent *ASC = GetOwningAbilitySystemComponent();
  if (!ASC)
    return;

  // 如果已经赋予过，不再赋予
  if (WeaponAbilityHandles.Contains(Weapon))
    return;

  FAbyssAbilitySetHandle HandleSet;

  // 1. 赋予主动技能
  for (const TSubclassOf<UGameplayAbility> &AbilityClass :
       Weapon->WeaponDef->AbilitiesToGrant) {
    if (AbilityClass) {
      FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, Weapon);
      FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
      HandleSet.Handles.Add(Handle);
    }
  }

  WeaponAbilityHandles.Add(Weapon, HandleSet);
}

void UAbyssInventoryComponent::RemoveWeaponAbilities(AAbyssWeaponBase *Weapon) {
  if (!Weapon || !GetOwner())
    return;

  UAbilitySystemComponent *ASC = GetOwningAbilitySystemComponent();
  if (!ASC)
    return;

  if (FAbyssAbilitySetHandle *HandleSet = WeaponAbilityHandles.Find(Weapon)) {
    for (const FGameplayAbilitySpecHandle &Handle : HandleSet->Handles) {
      ASC->ClearAbility(Handle);
    }
    WeaponAbilityHandles.Remove(Weapon);
  }
}
