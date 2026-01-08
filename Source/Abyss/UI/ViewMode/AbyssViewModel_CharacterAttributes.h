// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MVVMViewModelBase.h"

#include "AbyssViewModel_CharacterAttributes.generated.h"


class AAbyssPlayerController;
// ===============================
// VitalViewModel.h
// ===============================
UCLASS(BlueprintType)
class UVitalViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()
/*private:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float Health;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float MaxHealth;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float Mana;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float MaxMana;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float Stamina;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float MaxStamina;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float MoveSpeed;
	
public:
	//因为它是纯计算字段（不是UPROPERTY），必须用 UFUNCTION(BlueprintPure, FieldNotify) 才能在 MVVM 中绑定并响应事件。
	//FieldNotify 允许它接收手动广播的通知（来自 SetHealth / SetMaxHealth）。
	UFUNCTION(BlueprintPure, FieldNotify)
	float GetHealthPercent() const { return MaxHealth > 0.f ? Health / MaxHealth : 0.f; }
	UFUNCTION(BlueprintPure, FieldNotify)
	float GetManaPercent() const { return MaxMana > 0.f ? Mana / MaxMana : 0.f; }
	UFUNCTION(BlueprintPure, FieldNotify)
	float GetStaminaPercent() const { return MaxStamina > 0.f ? Stamina / MaxStamina : 0.f; }

	float GetHealth() const { return Health; }
	float GetMaxHealth() const { return MaxHealth; }
	float GetMana() const { return Mana; }
	float GetMaxMana() const { return MaxMana; }
	float GetStamina() const { return Stamina; }
	float GetMaxStamina() const { return MaxStamina; }
	float GetMoveSpeed() const { return MoveSpeed; }

	void SetHealth(float NewHealth)
	{
		//测试字段值是否已更改，然后设置字段的新值并广播事件: UE_MVVM_SET_PROPERTY_VALUE([成员名称], [新值]);
		if (UE_MVVM_SET_PROPERTY_VALUE(Health, NewHealth))
		{
			//广播事件: UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED([事件名称])
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHealthPercent);
		}
	}
	void SetMaxHealth(float NewMaxHealth)
	{
		if (UE_MVVM_SET_PROPERTY_VALUE(MaxHealth, NewMaxHealth))
		{
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHealthPercent);
		}
	}
	void SetMana(float NewMana)
	{
		if (UE_MVVM_SET_PROPERTY_VALUE(Mana, NewMana))
		{
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetManaPercent);
		}
	}
	void SetMaxMana(float NewMaxMana)
	{
		if (UE_MVVM_SET_PROPERTY_VALUE(MaxMana, NewMaxMana))
		{
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetManaPercent);
		}
	}
	void SetStamina(float NewStamina)
	{
		if (UE_MVVM_SET_PROPERTY_VALUE(Stamina, NewStamina))
		{
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetStaminaPercent);
		}
	}
	void SetMaxStamina(float NewMaxStamina)
	{
		if (UE_MVVM_SET_PROPERTY_VALUE(MaxStamina, NewMaxStamina))
		{
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetStaminaPercent);
		}
	}
	void SetMoveSpeed(float NewMoveSpeed)
	{
		UE_MVVM_SET_PROPERTY_VALUE(MoveSpeed, NewMoveSpeed);
	}*/
};

// ===============================
// OffenseViewModel.h
// ===============================
UCLASS(BlueprintType)
class UOffenseViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()
/*private:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float PhysicalPower;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float MagicPower;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float AttackSpeed;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float CooldownReduction;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float CritRate;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float CritDamage;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float PhysicalPenetration;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float MagicPenetration;
public:
	float GetPhysicalPower() const { return PhysicalPower; }
	float GetMagicPower() const { return MagicPower; }
	float GetAttackSpeed() const { return AttackSpeed; }
	float GetCooldownReduction() const { return CooldownReduction; }
	float GetCritRate() const { return CritRate; }
	float GetCritDamage() const { return CritDamage; }
	float GetPhysicalPenetration() const { return PhysicalPenetration; }
	float GetMagicPenetration() const { return MagicPenetration; }
	void SetPhysicalPower(float NewPhysicalPower) { UE_MVVM_SET_PROPERTY_VALUE(PhysicalPower, NewPhysicalPower); }
	void SetMagicPower(float NewMagicPower) { UE_MVVM_SET_PROPERTY_VALUE(MagicPower, NewMagicPower); }
	void SetAttackSpeed(float NewAttackSpeed) { UE_MVVM_SET_PROPERTY_VALUE(AttackSpeed, NewAttackSpeed); }
	void SetCooldownReduction(float NewCooldownReduction) { UE_MVVM_SET_PROPERTY_VALUE(CooldownReduction, NewCooldownReduction); }
	void SetCritRate(float NewCritRate) { UE_MVVM_SET_PROPERTY_VALUE(CritRate, NewCritRate); }
	void SetCritDamage(float NewCritDamage) { UE_MVVM_SET_PROPERTY_VALUE(CritDamage, NewCritDamage); }
	void SetPhysicalPenetration(float NewPhysicalPenetration) { UE_MVVM_SET_PROPERTY_VALUE(PhysicalPenetration, NewPhysicalPenetration); }
	void SetMagicPenetration(float NewMagicPenetration) { UE_MVVM_SET_PROPERTY_VALUE(MagicPenetration, NewMagicPenetration); }*/
};

// ===============================
// DefenseViewModel.h
// ===============================
UCLASS(BlueprintType)
class UDefenseViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()
/*private:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float Defense;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float BlockChance;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float PhysicalResistance;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float MagicResistance;
public:
	float GetDefense() const { return Defense; }
	float GetBlockChance() const { return BlockChance; }
	float GetPhysicalResistance() const { return PhysicalResistance; }
	float GetMagicResistance() const { return MagicResistance; }
	void SetDefense(float NewDefense) { UE_MVVM_SET_PROPERTY_VALUE(Defense, NewDefense); }
	void SetBlockChance(float NewBlockChance) { UE_MVVM_SET_PROPERTY_VALUE(BlockChance, NewBlockChance); }
	void SetPhysicalResistance(float NewPhysicalResistance) { UE_MVVM_SET_PROPERTY_VALUE(PhysicalResistance, NewPhysicalResistance); }
	void SetMagicResistance(float NewMagicResistance) { UE_MVVM_SET_PROPERTY_VALUE(MagicResistance, NewMagicResistance); }*/
};


/**
 * 
 */
UCLASS(BlueprintType)
class ABYSS_API UAbyssViewModel_CharacterAttributes : public UMVVMViewModelBase
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float Health;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float MaxHealth;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float Mana;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float MaxMana;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float Stamina;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float MaxStamina;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float MoveSpeed;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float PhysicalPower;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float MagicPower;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float AttackSpeed;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float CooldownReduction;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float CritRate;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float CritDamage;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float PhysicalPenetration;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float MagicPenetration;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float Defense;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float BlockChance;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float PhysicalResistance;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float MagicResistance;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	int32 SkillPoints;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	int32 SkillSlotAmount;

public:
	//因为它是纯计算字段（不是UPROPERTY），必须用 UFUNCTION(BlueprintPure, FieldNotify) 才能在 MVVM 中绑定并响应事件。
	//FieldNotify 允许它接收手动广播的通知（来自 SetHealth / SetMaxHealth）。
	UFUNCTION(BlueprintPure, FieldNotify)
	float GetHealthPercent() const { return MaxHealth > 0.f ? Health / MaxHealth : 0.f; }
	UFUNCTION(BlueprintPure, FieldNotify)
	float GetManaPercent() const { return MaxMana > 0.f ? Mana / MaxMana : 0.f; }
	UFUNCTION(BlueprintPure, FieldNotify)
	float GetStaminaPercent() const { return MaxStamina > 0.f ? Stamina / MaxStamina : 0.f; }

	float GetHealth() const { return Health; }
	float GetMaxHealth() const { return MaxHealth; }
	float GetMana() const { return Mana; }
	float GetMaxMana() const { return MaxMana; }
	float GetStamina() const { return Stamina; }
	float GetMaxStamina() const { return MaxStamina; }
	float GetMoveSpeed() const { return MoveSpeed; }

	void SetHealth(float NewHealth)
	{
		//测试字段值是否已更改，然后设置字段的新值并广播事件: UE_MVVM_SET_PROPERTY_VALUE([成员名称], [新值]);
		if (UE_MVVM_SET_PROPERTY_VALUE(Health, NewHealth))
		{
			//广播事件: UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED([事件名称])
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHealthPercent);
		}
	}
	void SetMaxHealth(float NewMaxHealth)
	{
		if (UE_MVVM_SET_PROPERTY_VALUE(MaxHealth, NewMaxHealth))
		{
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHealthPercent);
		}
	}
	void SetMana(float NewMana)
	{
		if (UE_MVVM_SET_PROPERTY_VALUE(Mana, NewMana))
		{
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetManaPercent);
		}
	}
	void SetMaxMana(float NewMaxMana)
	{
		if (UE_MVVM_SET_PROPERTY_VALUE(MaxMana, NewMaxMana))
		{
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetManaPercent);
		}
	}
	void SetStamina(float NewStamina)
	{
		if (UE_MVVM_SET_PROPERTY_VALUE(Stamina, NewStamina))
		{
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetStaminaPercent);
		}
	}
	void SetMaxStamina(float NewMaxStamina)
	{
		if (UE_MVVM_SET_PROPERTY_VALUE(MaxStamina, NewMaxStamina))
		{
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetStaminaPercent);
		}
	}
	void SetMoveSpeed(float NewMoveSpeed)
	{
		UE_MVVM_SET_PROPERTY_VALUE(MoveSpeed, NewMoveSpeed);
	}

	float GetPhysicalPower() const { return PhysicalPower; }
	float GetMagicPower() const { return MagicPower; }
	float GetAttackSpeed() const { return AttackSpeed; }
	float GetCooldownReduction() const { return CooldownReduction; }
	float GetCritRate() const { return CritRate; }
	float GetCritDamage() const { return CritDamage; }
	float GetPhysicalPenetration() const { return PhysicalPenetration; }
	float GetMagicPenetration() const { return MagicPenetration; }
	void SetPhysicalPower(float NewPhysicalPower) { UE_MVVM_SET_PROPERTY_VALUE(PhysicalPower, NewPhysicalPower); }
	void SetMagicPower(float NewMagicPower) { UE_MVVM_SET_PROPERTY_VALUE(MagicPower, NewMagicPower); }
	void SetAttackSpeed(float NewAttackSpeed) { UE_MVVM_SET_PROPERTY_VALUE(AttackSpeed, NewAttackSpeed); }
	void SetCooldownReduction(float NewCooldownReduction) { UE_MVVM_SET_PROPERTY_VALUE(CooldownReduction, NewCooldownReduction); }
	void SetCritRate(float NewCritRate) { UE_MVVM_SET_PROPERTY_VALUE(CritRate, NewCritRate); }
	void SetCritDamage(float NewCritDamage) { UE_MVVM_SET_PROPERTY_VALUE(CritDamage, NewCritDamage); }
	void SetPhysicalPenetration(float NewPhysicalPenetration) { UE_MVVM_SET_PROPERTY_VALUE(PhysicalPenetration, NewPhysicalPenetration); }
	void SetMagicPenetration(float NewMagicPenetration) { UE_MVVM_SET_PROPERTY_VALUE(MagicPenetration, NewMagicPenetration); }

	float GetDefense() const { return Defense; }
	float GetBlockChance() const { return BlockChance; }
	float GetPhysicalResistance() const { return PhysicalResistance; }
	float GetMagicResistance() const { return MagicResistance; }
	void SetDefense(float NewDefense) { UE_MVVM_SET_PROPERTY_VALUE(Defense, NewDefense); }
	void SetBlockChance(float NewBlockChance) { UE_MVVM_SET_PROPERTY_VALUE(BlockChance, NewBlockChance); }
	void SetPhysicalResistance(float NewPhysicalResistance) { UE_MVVM_SET_PROPERTY_VALUE(PhysicalResistance, NewPhysicalResistance); }
	void SetMagicResistance(float NewMagicResistance) { UE_MVVM_SET_PROPERTY_VALUE(MagicResistance, NewMagicResistance); }
	
	
	int32 GetSkillPoints() const { return SkillPoints; }
	void SetSkillPoints(int32 NewSkillPoints) { UE_MVVM_SET_PROPERTY_VALUE(SkillPoints, NewSkillPoints); }
	int32 GetSkillSlotAmount() const { return SkillSlotAmount; }
	void SetSkillSlotAmount(int32 NewSkillSlotAmount) { UE_MVVM_SET_PROPERTY_VALUE(SkillSlotAmount, NewSkillSlotAmount); }
};

