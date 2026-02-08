// Copyright (c) 2025 Leon Lee

#pragma once

#include "AbilitySystem/Abilities/Projectiles/AbyssProjectileAbility.h"
#include "CoreMinimal.h"
#include "AbyssGameplayAbility_FireWeapon.generated.h"
class UAnimMontage;

/**
 *
 */
UCLASS()
class ABYSS_API UAbyssGameplayAbility_FireWeapon
    : public UAbyssProjectileAbility {
  GENERATED_BODY()
public:
  UAbyssGameplayAbility_FireWeapon();

  // GAS 标准入口
  virtual void
  ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                  const FGameplayAbilityActorInfo *ActorInfo,
                  const FGameplayAbilityActivationInfo ActivationInfo,
                  const FGameplayEventData *TriggerEventData) override;

  // GAS 标准结束
  virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
                          const FGameplayAbilityActorInfo *ActorInfo,
                          const FGameplayAbilityActivationInfo ActivationInfo,
                          bool bReplicateEndAbility,
                          bool bWasCancelled) override;

  // 监听按键松开 (这是实现"松手停止"的关键)
  virtual void
  InputReleased(const FGameplayAbilitySpecHandle Handle,
                const FGameplayAbilityActorInfo *ActorInfo,
                const FGameplayAbilityActivationInfo ActivationInfo) override;

  // 【关键修复】重写父类的SpawnProjectile，使用缓存的旋转
  virtual void
  SpawnProjectile(FVector ProjectileTargetLocation = FVector::ZeroVector,
                  FGameplayTag SocketTag = FGameplayTag(),
                  bool bOverridePitch = false,
                  float PitchOverride = 0.f) override;

protected:
  // 真正的开火逻辑 (将被定时器反复调用)
  void FireShot();

  // 定时器句柄
  FTimerHandle TimerHandle_AutoFire;

  // 缓存的射击方向 (在Activate时捕获，用于服务器端复现客户端预测)
  FRotator CachedFireRotation;

  // 防止自动换弹被重复触发的标志
  bool bAutoReloadTriggered = false;
};
