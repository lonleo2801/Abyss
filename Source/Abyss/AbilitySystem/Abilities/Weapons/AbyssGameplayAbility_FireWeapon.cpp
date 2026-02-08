// Copyright (c) 2025 Leon Lee

#include "AbilitySystem/Abilities/Weapons/AbyssGameplayAbility_FireWeapon.h"
#include "AbilitySystem/Abilities/Projectiles/AbyssProjectileAbility.h"
#include "AbilitySystemComponent.h"
#include "Actors/Projectiles/AbyssProjectile.h"
#include "Actors/Weapons/AbyssWeaponBase.h"
#include "Characters/AbyssPawn.h"
#include "Components/Gameplay/AbyssInventoryComponent.h"
#include "GameplayTags/AbyssTags.h"
#include "Interfaces/AbyssCombatInterface.h"

UAbyssGameplayAbility_FireWeapon::UAbyssGameplayAbility_FireWeapon() {

  NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

  InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UAbyssGameplayAbility_FireWeapon::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData *TriggerEventData) {
  // 重置自动换弹标志
  bAutoReloadTriggered = false;

  // 1. 基础检查 (不消耗 Cost，只检查能不能放)
  if (!CommitCheck(Handle, ActorInfo, ActivationInfo)) {
    EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    return;
  }

  // 2. 获取武器和射速
  AAbyssPawn *AvatarPawn = Cast<AAbyssPawn>(GetAvatarActorFromActorInfo());
  UAbyssInventoryComponent *Inventory =
      AvatarPawn ? AvatarPawn->GetInventoryComponent() : nullptr;
  AAbyssWeaponBase *Weapon =
      Inventory ? Inventory->GetCurrentWeapon() : nullptr;

  if (!Weapon || !Weapon->WeaponDef) {
    EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    return;
  }

  // 获取射击间隔 (TimeBetweenShots)
  // 假设你的 WeaponDef 里有个变量叫 TimeBetweenShots (例如 0.1s 代表 1秒10发)
  float TimeBetweenShots = Weapon->WeaponDef->TimeBetweenShots;

  // 保护一下，防止除以0或过快
  if (TimeBetweenShots <= 0.0f)
    TimeBetweenShots = 0.1f;

  // 【关键修复】在 Activate 时捕获射击方向，确保服务器使用客户端发来的旋转
  // 这样在 LocalPredicted 模式下，服务器会复现客户端的准星方向
  CachedFireRotation = GetControlRotationSafe();

  // 3. 立即发射第一发
  FireShot();

  // 4. 启动定时器 (Looping = true)
  // 只有当 Ability 还在激活状态时，定时器才会持续运行
  GetWorld()->GetTimerManager().SetTimer(
      TimerHandle_AutoFire, this, &UAbyssGameplayAbility_FireWeapon::FireShot,
      TimeBetweenShots, true);

  // 注意：这里不要调用 Super::ActivateAbility，或者 EndAbility
  // 我们希望 Ability 保持 Active 状态，直到玩家松手
}

void UAbyssGameplayAbility_FireWeapon::FireShot() {
  // 这是一个回调函数，环境可能已经变了（比如武器被丢了），所以要重新检查
  // 如果想要每发子弹都消耗 Stamina 或其他 Cost，就在这里调用 CommitAbility
  /*if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo,
  CurrentActivationInfo))
  {
      // 如果 Cost 不足 (比如没蓝了/没体力了)，停止射击
      EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
  true, false); return;
  }*/

  AAbyssPawn *AvatarPawn = Cast<AAbyssPawn>(GetAvatarActorFromActorInfo());
  UAbyssInventoryComponent *Inventory =
      AvatarPawn ? AvatarPawn->GetInventoryComponent() : nullptr;
  AAbyssWeaponBase *Weapon =
      Inventory ? Inventory->GetCurrentWeapon() : nullptr;

  // 再次安全检查
  if (!Weapon) {
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true,
               true);
    return;
  }

  // 1. 检查弹药
  if (Weapon->GetCurrentAmmo() <= 0) {
    // 弹药耗尽，停止连射
    CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
                  true);

    // 自动触发装弹
    // 由于这是 LocalPredicted 能力，FireShot 在客户端和服务器都会执行
    // 为避免双重触发换弹，只在"负责预测的端"触发：
    // - 普通客户端：IsLocallyControlled() && !HasAuthority() -> 触发
    // - Listen Server Host：IsLocallyControlled() && HasAuthority() ->
    // 触发（只有一个实例）
    // - Dedicated Server 上的远程玩家：!IsLocallyControlled() ->
    // 不触发（等客户端发来的激活请求）
    const bool bIsServer = GetOwningActorFromActorInfo()->HasAuthority();
    const bool bIsLocallyControlled =
        AvatarPawn && AvatarPawn->IsLocallyControlled();

    // 只有本地控制的玩家才主动触发换弹
    // 并且使用标志防止重复触发（客户端和服务器的 FireShot
    // 可能因为时序问题都检测到弹药=0）
    if (bIsLocallyControlled && !bAutoReloadTriggered) {
      bAutoReloadTriggered = true;
      if (UAbilitySystemComponent *ASC =
              GetAbilitySystemComponentFromActorInfo()) {
        FGameplayTagContainer ReloadTags;
        ReloadTags.AddTag(AbyssTags::Ability::Weapon::Reload);
        ASC->TryActivateAbilitiesByTag(ReloadTags, false);
      }
    }
    return;
  }

  // 2. 消耗弹药 (Server Only)
  if (GetOwningActorFromActorInfo()->HasAuthority()) {
    Weapon->ConsumeAmmo(1);
  }

  // 3. 播放动画 (蒙太奇)
  // 注意：如果是高射速武器，每次都 PlayMontage 可能会导致动画鬼畜
  // 通常高射速武器只在 Activate 播放 Start，End 播放 Stop，或者只播放上半身
  // Layer
  /*if (FireMontage) {
      MontagePlay(FireMontage, 1.f);
  }*/

  // 4. 每次射击前更新缓存的旋转，确保连射时方向跟随准星
  //    对于本地控制的 Pawn，获取最新控制旋转
  //    对于服务器上的模拟 Pawn，使用复制的控制旋转或 Actor 旋转
  if (AAbyssPawn *Pawn = Cast<AAbyssPawn>(GetAvatarActorFromActorInfo())) {
    if (Pawn->IsLocallyControlled()) {
      CachedFireRotation = GetControlRotationSafe();
    } else {
      // 服务器上非本地控制的 Pawn，使用其复制的控制旋转
      CachedFireRotation = Pawn->GetBaseAimRotation();
    }
  }

  // 5. 发射投射物
  SpawnProjectile(FVector::ZeroVector, FGameplayTag(), false, 0.f);
}

// 【关键修复】重写 SpawnProjectile 以使用缓存的旋转
void UAbyssGameplayAbility_FireWeapon::SpawnProjectile(
    FVector ProjectileTargetLocation, FGameplayTag SocketTag,
    bool bOverridePitch, float PitchOverride) {
  const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
  if (!bIsServer)
    return;

  IAbyssCombatInterface *CombatInterface =
      Cast<IAbyssCombatInterface>(GetAvatarActorFromActorInfo());
  if (CombatInterface) {
    const FVector SocketLocation =
        GetAvatarActorFromActorInfo()->GetActorLocation();

    FRotator Rotation;
    if (ProjectileTargetLocation.IsZero()) {
      // 【关键】使用缓存的旋转而非实时查询
      // 这样服务器会使用客户端发来的预测旋转
      Rotation = CachedFireRotation;
    } else {
      Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
    }

    if (bOverridePitch) {
      Rotation.Pitch = PitchOverride;
    } else {
      Rotation.Pitch = 0;
    }

    FTransform SpawnTransform;
    SpawnTransform.SetLocation(SocketLocation);
    SpawnTransform.SetRotation(Rotation.Quaternion());

    AAbyssProjectile *Projectile =
        GetWorld()->SpawnActorDeferred<AAbyssProjectile>(
            ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(),
            Cast<APawn>(GetOwningActorFromActorInfo()),
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

    Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

    Projectile->FinishSpawning(SpawnTransform);
  }
}

void UAbyssGameplayAbility_FireWeapon::InputReleased(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo) {
  Super::InputReleased(Handle, ActorInfo, ActivationInfo);

  // 当玩家松开鼠标左键时，结束 Ability
  EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UAbyssGameplayAbility_FireWeapon::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility, bool bWasCancelled) {
  // 无论是因为松手、没子弹、还是被眩晕打断，只要 Ability 结束，必须清理定时器
  GetWorld()->GetTimerManager().ClearTimer(TimerHandle_AutoFire);

  Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility,
                    bWasCancelled);
}