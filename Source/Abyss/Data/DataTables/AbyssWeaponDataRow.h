// Copyright (c) 2025 Leon Lee
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "AbyssWeaponDataRow.generated.h"

class UGameplayAbility;
/**
 * 武器数据结构体（用于 DataRegistry）
 */
USTRUCT(BlueprintType)
struct FAbyssWeaponDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 唯一武器Tag（比如 Weapon.Sword.Long）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	FGameplayTag WeaponTag;
	
	/** 武器显示名称 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;
	
	// 攻击能力类（用于GAS）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	TSubclassOf<UGameplayAbility> AttackAbility;

	// 模型引用（SkeletalMesh）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	TSoftObjectPtr<USkeletalMesh> Mesh;

	// 是否双手武器
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	bool bTwoHanded = false;

	/** 攻击力 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Damage = 0.f;

	/** 攻击速度 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackSpeed = 1.f;

	/** 重量（影响移动速度） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight = 1.f;

	/** 价格 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Price = 0;
	
};
