// Copyright (c) 2025 Leon Lee

#include "AbyssInventoryComponent.h"

#include "AbilitySystem/AbyssAbilitySystemComponent.h"
#include "AbilitySystemComponent.h"

#include "Actors/Weapons/AbyssWeaponBase.h"
#include "Components/Input/AbyssInputComponent.h"
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

  DOREPLIFETIME(UAbyssInventoryComponent, RifleReserveAmmo);
  DOREPLIFETIME(UAbyssInventoryComponent, PistolReserveAmmo);
}

void UAbyssInventoryComponent::BeginPlay() {
  Super::BeginPlay();

  if (GetOwningPawn()->IsLocallyControlled() &&
      GetOwningPawn()->InputComponent) {
    if (UAbyssInputComponent *AbyssEnhancedInputComponent =
            Cast<UAbyssInputComponent>(GetOwningPawn()->InputComponent)) {
      // 绑定 "按下" (Started)
      AbyssEnhancedInputComponent->BindAction(
          PrimaryWeaponsAction, ETriggerEvent::Started, this,
          &ThisClass::OnPrimaryWeaponsAction);
      AbyssEnhancedInputComponent->BindAction(
          SecondaryWeaponsAction, ETriggerEvent::Started, this,
          &ThisClass::OnSecondaryWeaponsAction);
      AbyssEnhancedInputComponent->BindAction(MeleeWeaponsAction,
                                              ETriggerEvent::Started, this,
                                              &ThisClass::OnMeleeWeaponsAction);
      AbyssEnhancedInputComponent->BindAction(
          PreviousWeaponAction, ETriggerEvent::Started, this,
          &ThisClass::OnPreviousWeaponAction);
      AbyssEnhancedInputComponent->BindAction(DropWeaponAction,
                                              ETriggerEvent::Started, this,
                                              &ThisClass::OnDropWeaponAction);
    }
  }
}

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
    OnInventoryUpdated.Broadcast();

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

  // 1. Unbind from old weapon's ammo delegate
  if (OldWeapon) {
    if (OldWeapon->OnClipAmmoChanged.IsAlreadyBound(
            this, &ThisClass::OnCurrentWeaponAmmoUpdate)) {
      OldWeapon->OnClipAmmoChanged.RemoveDynamic(
          this, &ThisClass::OnCurrentWeaponAmmoUpdate);
    }
  }

  // 2. Bind to new weapon's ammo delegate
  if (CurrentWeapon) {
    if (!CurrentWeapon->OnClipAmmoChanged.IsAlreadyBound(
            this, &ThisClass::OnCurrentWeaponAmmoUpdate)) {
      CurrentWeapon->OnClipAmmoChanged.AddDynamic(
          this, &ThisClass::OnCurrentWeaponAmmoUpdate);
    }
  }

  // 3. Broadcast change
  OnInventoryUpdated.Broadcast();
  OnAmmoChanged.Broadcast();
}

void UAbyssInventoryComponent::OnCurrentWeaponAmmoUpdate(int32 NewAmmo) {
  // Broadcast clip ammo change to HUD
  OnAmmoChanged.Broadcast();
}

void UAbyssInventoryComponent::GrantWeaponAbilities(AAbyssWeaponBase *Weapon) {
  if (!Weapon || !Weapon->WeaponDef || !GetOwner())
    return;

  UAbyssAbilitySystemComponent *ASC =
      Cast<UAbyssAbilitySystemComponent>(GetOwningAbilitySystemComponent());
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
      FGameplayAbilitySpecHandle Handle =
          ASC->AddCharacterAbility(AbilityClass);
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

void UAbyssInventoryComponent::OnPrimaryWeaponsAction(
    const FInputActionValue &Value) {
  ServerEquipWeapon(EWeaponSlot::Primary);
}

void UAbyssInventoryComponent::OnSecondaryWeaponsAction(
    const FInputActionValue &Value) {
  ServerEquipWeapon(EWeaponSlot::Secondary);
}

void UAbyssInventoryComponent::OnMeleeWeaponsAction(
    const FInputActionValue &Value) {
  ServerEquipWeapon(EWeaponSlot::Melee);
}

void UAbyssInventoryComponent::OnPreviousWeaponAction(
    const FInputActionValue &Value) {
  ServerSwapToPreviousWeapon();
}

void UAbyssInventoryComponent::OnDropWeaponAction(
    const FInputActionValue &Value) {
  ServerDropWeapon();
}

// ============================================================================
// Ammo System
// ============================================================================

int32 UAbyssInventoryComponent::GetReserveAmmo(EAmmoType AmmoType) const {
  if (AmmoType == EAmmoType::Rifle) {
    return RifleReserveAmmo;
  } else if (AmmoType == EAmmoType::Pistol) {
    return PistolReserveAmmo;
  }

  return 0;
}

void UAbyssInventoryComponent::AddReserveAmmo(EAmmoType AmmoType,
                                              int32 Amount) {
  if (Amount <= 0) {
    return;
  }

  int32 NewAmount = 0;
  if (AmmoType == EAmmoType::Rifle) {
    RifleReserveAmmo =
        FMath::Clamp(RifleReserveAmmo + Amount, 0, MaxRifleReserveAmmo);
    NewAmount = RifleReserveAmmo;
  } else if (AmmoType == EAmmoType::Pistol) {
    PistolReserveAmmo =
        FMath::Clamp(PistolReserveAmmo + Amount, 0, MaxPistolReserveAmmo);
    NewAmount = PistolReserveAmmo;
  }

  if (GetOwner() && GetOwner()->HasAuthority()) {
    OnAmmoChanged.Broadcast();
  }
}

bool UAbyssInventoryComponent::ConsumeReserveAmmo(EAmmoType AmmoType,
                                                  int32 Amount) {
  if (Amount <= 0) {
    return false;
  }

  int32 Current = GetReserveAmmo(AmmoType);
  if (Current >= Amount) {
    int32 NewAmount = 0;
    if (AmmoType == EAmmoType::Rifle) {
      RifleReserveAmmo =
          FMath::Clamp(RifleReserveAmmo - Amount, 0, MaxRifleReserveAmmo);
      NewAmount = RifleReserveAmmo;
    } else if (AmmoType == EAmmoType::Pistol) {
      PistolReserveAmmo =
          FMath::Clamp(PistolReserveAmmo - Amount, 0, MaxPistolReserveAmmo);
      NewAmount = PistolReserveAmmo;
    }

    if (GetOwner() && GetOwner()->HasAuthority()) {
      OnAmmoChanged.Broadcast();
    }
    return true;
  }
  return false;
}

EAmmoType UAbyssInventoryComponent::GetAmmoTypeForSlot(EWeaponSlot Slot) const {
  if (Slot == EWeaponSlot::Primary) {
    return EAmmoType::Rifle;
  }
  if (Slot == EWeaponSlot::Secondary) {
    return EAmmoType::Pistol;
  }
  return EAmmoType::None;
}

void UAbyssInventoryComponent::OnRep_Inventory() {
  OnInventoryUpdated.Broadcast();
}

void UAbyssInventoryComponent::OnRep_RifleReserveAmmo() {
  OnAmmoChanged.Broadcast();
}

void UAbyssInventoryComponent::OnRep_PistolReserveAmmo() {
  OnAmmoChanged.Broadcast();
}
