// Copyright (c) 2025 Leon Lee


#include "AbyssInventoryComponent.h"

#include "AbilitySystemComponent.h"

#include "Actors/Weapons/AbyssWeaponBase.h"
#include "Net/UnrealNetwork.h"


UAbyssInventoryComponent::UAbyssInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UAbyssInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UAbyssInventoryComponent, InventoryList);
    DOREPLIFETIME(UAbyssInventoryComponent, CurrentWeapon);
    DOREPLIFETIME(UAbyssInventoryComponent, CurrentSlotType);
}

void UAbyssInventoryComponent::BeginPlay()
{
    Super::BeginPlay();
}

// ============================================================================
// 拾取与自动装备逻辑 (Server Authority)
// ============================================================================
void UAbyssInventoryComponent::ServerPickupWeapon_Implementation(AAbyssWeaponBase* WeaponToPickup)
{
    if (!WeaponToPickup || !GetOwner() || !WeaponToPickup->WeaponDef) return;

    // 1. 获取新武器的槽位类型
    EWeaponSlot TargetSlot = WeaponToPickup->WeaponDef->SlotType;
    
    // 2. 检查该槽位是否已经有武器了
    AAbyssWeaponBase* ExistingWeapon = GetWeaponInSlot(TargetSlot);
    
    bool bWasSlotEmpty = (ExistingWeapon == nullptr); // 记录该槽位之前是否为空

    // 3. 如果槽位已有武器 -> 替换逻辑
    if (ExistingWeapon)
    {
        // 如果我们正在拿这把旧武器，先丢弃它 (这会将 CurrentWeapon 置空)
        if (ExistingWeapon == CurrentWeapon)
        {
            ServerDropWeapon(); 
        }
        else
        {
            // 如果它在背上，直接销毁或者扔在地上 (这里简化为扔在地上)
            RemoveWeaponAbilities(ExistingWeapon);
            InventoryList.Remove(ExistingWeapon);
    
            // 先分离并设置位置，再开启物理
            ExistingWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
            ExistingWeapon->SetActorLocation(GetOwner()->GetActorLocation() + FVector(0, 0, 50.f));
            ExistingWeapon->SetWeaponState(EWeaponState::OnGround);
            ExistingWeapon->SetOwner(nullptr);
    
            // 给冲量
            if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(ExistingWeapon->GetRootComponent()))
            {
                Root->AddImpulse(GetOwner()->GetActorForwardVector() * 200.f, NAME_None, true);
            }
        }
    }

    // 4. 正式接收新武器
    WeaponToPickup->SetOwner(GetOwner());
    InventoryList.Add(WeaponToPickup);
    GrantWeaponAbilities(WeaponToPickup);

    // ========================================================================
    // 自动装备判定 (Auto Equip Logic)
    // ========================================================================
    // 触发条件：
    // A. 当前手里什么都没有 (CurrentWeapon == nullptr)
    // B. 或者，根据你的需求：如果这个槽位之前是空的，就强制切过去 (可选)
    //    比如：手里拿着手枪，捡到第一把长枪，是否自动切长枪？TLOU2通常是会的。
    
    if (CurrentWeapon == nullptr || bWasSlotEmpty)
    {
        // 自动装备到对应槽位
        ServerEquipWeapon(TargetSlot);
    }
    else
    {
        // 否则，默默挂在身上 (Holstered)
        WeaponToPickup->SetWeaponState(EWeaponState::Holstered);
    }
}

// ============================================================================
// 装备/切换逻辑 (Server Authority)
// ============================================================================
void UAbyssInventoryComponent::ServerEquipWeapon_Implementation(EWeaponSlot SlotToEquip)
{
    // 1. 找到该槽位的武器
    AAbyssWeaponBase* WeaponToEquip = GetWeaponInSlot(SlotToEquip);

    // 如果该槽位没枪，或者是当前正在拿的枪，直接返回
    if (!WeaponToEquip || WeaponToEquip == CurrentWeapon) return;

    // 2. 记录旧武器
    AAbyssWeaponBase* OldWeapon = CurrentWeapon;

    // 3. 收起旧武器
    if (OldWeapon)
    {
        OldWeapon->SetWeaponState(EWeaponState::Holstered);
    }

    // 4. 拿出新武器
    PreviousWeapon = OldWeapon; 
    CurrentWeapon = WeaponToEquip;
    CurrentSlotType = SlotToEquip; // 更新当前槽位枚举
    CurrentWeapon->SetWeaponState(EWeaponState::Equipped);

    // 5. 通知客户端 (Server端手动触发，Client端通过OnRep触发)
    OnRep_CurrentWeapon(OldWeapon);
}


// ============================================================================
// 丢弃逻辑 (Server Authority)
// ============================================================================
void UAbyssInventoryComponent::ServerDropWeapon_Implementation()
{
    if (!CurrentWeapon) return;

    AAbyssWeaponBase* WeaponToDrop = CurrentWeapon;

    // 1. 移除 GAS 能力
    RemoveWeaponAbilities(WeaponToDrop);

    // 2. 从列表移除
    InventoryList.Remove(WeaponToDrop);

    // 3. 改变状态 -> OnGround (物理模拟在 Weapon 类里处理)
    WeaponToDrop->SetWeaponState(EWeaponState::OnGround);

    // 4. 剥离所有权
    WeaponToDrop->SetOwner(nullptr);

    // 5. 给予一个向前的冲量 (扔出去)
    if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(WeaponToDrop->GetRootComponent()))
    {
        FVector ThrowDir = GetOwner()->GetActorForwardVector() + FVector(0, 0, 0.2f);
        Root->AddImpulse(ThrowDir * 200.f, NAME_None, true);
    }

    // 6. 清空指针
    PreviousWeapon = nullptr; // 避免切回空指针
    CurrentWeapon = nullptr;
    
    // Server 端通知
    OnCurrentWeaponChanged.Broadcast(nullptr);
}

// ============================================================================
// 客户端视觉同步 (Client & Server)
// ============================================================================
void UAbyssInventoryComponent::OnRep_CurrentWeapon(AAbyssWeaponBase* OldWeapon)
{
    // 注意：AWeaponBase::OnRep_State 已经处理了 Attach Mesh 到 Socket 的逻辑
    // 这里主要处理 "角色" 层面的逻辑，比如 UI 更新、动画状态机变量设置
    
    //TODO:  广播给 HUD
    OnCurrentWeaponChanged.Broadcast(CurrentWeapon);

    // 如果你有动画层 (AnimInstance)，这里是更新 "IsRifle?", "IsPistol?" 变量的好地方
    if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
    {
        // 示例：更新 AnimInstance
        // UMyAnimInstance* AnimInst = Cast<UMyAnimInstance>(Character->GetMesh()->GetAnimInstance());
        // if (AnimInst) AnimInst->SetCurrentWeapon(CurrentWeapon);
    }
}

// ============================================================================
// GAS 集成
// ============================================================================
void UAbyssInventoryComponent::GrantWeaponAbilities(AAbyssWeaponBase* Weapon)
{
    if (!Weapon || !Weapon->WeaponDef) return;

    IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(GetOwner());
    if (!ASCInterface) return;
    
    UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
    if (!ASC) return;

    // 确保没有重复赋予
    if (WeaponAbilityHandles.Contains(Weapon)) return;
    
    FAbyssAbilitySetHandle HandleWrapper;

    for (const TSubclassOf<UGameplayAbility>& AbilityClass : Weapon->WeaponDef->AbilitiesToGrant)
    {
        if (AbilityClass)
        {
            // 给 Ability 一个 SourceObject (Weapon)，这很关键！
            // 这样在 Ability 内部可以通过 GetSourceObject 获取弹夹数据
            FGameplayAbilitySpec Spec(AbilityClass, 1, -1, Weapon);
            FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
            
            HandleWrapper.Handles.Add(Handle);
        }
    }

    WeaponAbilityHandles.Add(Weapon, HandleWrapper);
}

void UAbyssInventoryComponent::RemoveWeaponAbilities(AAbyssWeaponBase* Weapon)
{
    if (!Weapon) return;

    IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(GetOwner());
    if (!ASCInterface) return;
    UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();

    if (ASC && WeaponAbilityHandles.Contains(Weapon))
    {
        const FAbyssAbilitySetHandle& HandleWrapper = WeaponAbilityHandles[Weapon];
        // 遍历结构体里的数组
        for (const FGameplayAbilitySpecHandle& Handle : HandleWrapper.Handles)
        {
            ASC->ClearAbility(Handle);
        }
        
        WeaponAbilityHandles.Remove(Weapon);
    }
}

// ============================================================================
// 工具函数
// ============================================================================
AAbyssWeaponBase* UAbyssInventoryComponent::GetWeaponInSlot(EWeaponSlot Slot) const
{
    for (AAbyssWeaponBase* Weapon : InventoryList)
    {
        // 确保 Weapon 和 WeaponDef 有效
        if (Weapon && Weapon->WeaponDef && Weapon->WeaponDef->SlotType == Slot)
        {
            return Weapon;
        }
    }
    return nullptr;
}


