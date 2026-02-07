// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "WeaponDefinition.generated.h"

class UCurveVector;
class UGameplayEffect;
class UGameplayAbility;

UENUM(BlueprintType)
enum class EWeaponSlot : uint8 {
  Primary UMETA(DisplayName = "Primary (Rifle)"),
  Secondary UMETA(DisplayName = "Secondary (Pistol)"),
  Melee UMETA(DisplayName = "Melee")
};

UENUM(BlueprintType)
enum class EAmmoType : uint8 {
  None UMETA(DisplayName = "None"),
  Rifle UMETA(DisplayName = "Rifle Ammo"),
  Pistol UMETA(DisplayName = "Pistol Ammo")
};

/**
 *
 */
UCLASS()
class ABYSS_API UWeaponDefinition : public UPrimaryDataAsset {
  GENERATED_BODY()

public:
  // ========================================================================
  // 1. 视觉与模型 (Visuals)
  // ========================================================================

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
  FText DisplayName; // 比如 "Pump Shotgun"

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
  UTexture2D *Icon; // UI 显示用的图标

  // 武器的骨骼模型 (必须是 SkeletalMesh，因为要有枪栓运动/弹夹骨骼)
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
  USkeletalMesh *WeaponMesh;

  // 装备时挂在哪里？(例如: "Hand_R")
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|Sockets")
  FName EquippedSocketName = FName("Hand_R");

  // 收起时挂在哪里？(例如: "Holster_Back", "Holster_Hip")
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|Sockets")
  FName HolsteredSocketName = FName("Holster_Back");

  // ========================================================================
  // 2. 游戏逻辑 (Gameplay Logic)
  // ========================================================================

  // 这把枪属于哪个槽位？
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
  EWeaponSlot SlotType;

  // 默认弹夹容量
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
  int32 DefaultClipSize = 30;

  // 武器的身份 Tag (例如: Weapon.Rifle.AK47)
  // 用于在动画蓝图中切换姿态，或在伤害计算中判断武器类型
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
  FGameplayTag WeaponTag;

  // 弹药类型 Tag (例如: Ammo.Rifle.762)
  // 用于判断背包里扣除哪种弹药
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
  FGameplayTag AmmoType;

  // ========================================================================
  // 3. GAS 能力集 (Abilities)
  // ========================================================================

  // 装备此武器时，赋予角色的主动技能
  // 通常包括: GA_Fire, GA_Reload, GA_ADS (瞄准)
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
  TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant;

  // 装备此武器时，赋予角色的被动效果
  // 例如: 增加后坐力属性，降低移动速度，改变摄像机FOV
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
  TArray<TSubclassOf<UGameplayEffect>> PassiveEffects;

  // ========================================================================
  // 4. 动画表现 (Animation) - TLOU2 风格核心
  // ========================================================================

  // 动画层接口 (Linked Anim Layer)
  // 当装备这把枪时，角色的动画蓝图会 Link 这个 Layer
  // 这样你就不用在一个巨大的 AnimBP 里写 switch(WeaponType)
  // 而是每把枪自带一套姿态动画（站立、跑动、瞄准）
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
  TSubclassOf<UAnimInstance> EquippedAnimLayer;

  // 武器自身的动画蓝图 (用于处理滑套后坐力、弹夹掉落)
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
  TSubclassOf<UAnimInstance> WeaponAnimBP;

  // ========================================================================
  // 5. 弹道与后坐力 (Ballistics - 可选，如果不放在 GE 里)
  // ========================================================================

  // 射击间隔 (秒)
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballistics")
  float TimeBetweenShots = 0.1f;

  // 散布曲线
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballistics")
  UCurveFloat *SpreadCurve;

  // 后坐力曲线 (用于 Control Rig)
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballistics")
  UCurveVector *RecoilCurve;
};
