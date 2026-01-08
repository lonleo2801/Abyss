// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "AbyssAttributeSet.generated.h"


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties() {}

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC=nullptr;
	UPROPERTY()
	AActor*  SourceAvatarActor=nullptr;
	UPROPERTY()
	AController* SourceController=nullptr;
	UPROPERTY()
	ACharacter* SourceCharacter=nullptr;


	UPROPERTY()
	UAbilitySystemComponent* TargetASC=nullptr;
	UPROPERTY()
	AActor* TargetAvatarActor=nullptr;
	UPROPERTY()
	AController* TargetController=nullptr;
	UPROPERTY()
	ACharacter* TargetCharacter=nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttributesInitialized);


template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;//Mapping Tags to Attributes


UCLASS()
class ABYSS_API UAbyssAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UAbyssAttributeSet();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributesMaps;
	
private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
	void ShowFloatingText(const FEffectProperties& EffectProperties,float Damage,bool bBlockHit,bool bCritHit) const;
	void HandleIncomingDamage(const FEffectProperties& EffectProperties);
	void HandleBuff(const FEffectProperties& EffectProperties);
	void HandleIncomingXP(const FEffectProperties& EffectProperties);
	void SendXPEvent(const FEffectProperties& Props);
	
public:	
	// -------------------------
    // 生存属性
    // -------------------------
    /** 当前生命值；复制以便客户端更新 UI（使用 RepNotify） */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UAbyssAttributeSet, Health)

    /** 最大生命值（可被 GE 修改） */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UAbyssAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(ThisClass, Mana);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(ThisClass, MaxMana);

    // -------------------------
    // 耐力/能量
    // -------------------------
    /** 当前体力/耐力 */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina)
    FGameplayAttributeData Stamina;
    ATTRIBUTE_ACCESSORS(UAbyssAttributeSet, Stamina)

    /** 最大体力 */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina)
    FGameplayAttributeData MaxStamina;
    ATTRIBUTE_ACCESSORS(UAbyssAttributeSet, MaxStamina)

    // -------------------------
    // 战斗属性
    // -------------------------
    /** 基础攻击力（用于伤害计算） */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalPower)
    FGameplayAttributeData PhysicalPower;
    ATTRIBUTE_ACCESSORS(UAbyssAttributeSet, PhysicalPower)

	/** 法强（用于伤害计算） */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicPower)
	FGameplayAttributeData MagicPower;
	ATTRIBUTE_ACCESSORS(ThisClass, MagicPower);

    /** 防御值（用于伤害减免） */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Defense)
    FGameplayAttributeData Defense;
    ATTRIBUTE_ACCESSORS(UAbyssAttributeSet, Defense)

    /** 暴击率（0..1） */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritRate)
    FGameplayAttributeData CritRate;
    ATTRIBUTE_ACCESSORS(UAbyssAttributeSet, CritRate)

    /** 暴击倍数（如 1.5 表示 150%） */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritDamage)
    FGameplayAttributeData CritDamage;
    ATTRIBUTE_ACCESSORS(UAbyssAttributeSet, CritDamage)

	/** 闪避几率 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BlockChance)
	FGameplayAttributeData BlockChance;
	ATTRIBUTE_ACCESSORS(ThisClass, BlockChance);
	
	/** 法术穿透 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicPenetration)
	FGameplayAttributeData MagicPenetration;
	ATTRIBUTE_ACCESSORS(ThisClass, MagicPenetration);

	/** 物理穿透 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalPenetration)
	FGameplayAttributeData PhysicalPenetration;
	ATTRIBUTE_ACCESSORS(ThisClass, PhysicalPenetration);

    // -------------------------
    // 表现类属性
    // -------------------------
    /** 攻击速度倍率（用于缩放动画或逻辑） */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackSpeed)
    FGameplayAttributeData AttackSpeed;
    ATTRIBUTE_ACCESSORS(UAbyssAttributeSet, AttackSpeed)

    /** 移动速度（单位：UU/s） */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MoveSpeed)
    FGameplayAttributeData MoveSpeed;
    ATTRIBUTE_ACCESSORS(UAbyssAttributeSet, MoveSpeed)

    // -------------------------
    // 扩展 / 保留字段（根据需要添加）
    // -------------------------
    /** 冷却缩减（0..1） */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CooldownReduction)
    FGameplayAttributeData CooldownReduction;
    ATTRIBUTE_ACCESSORS(UAbyssAttributeSet, CooldownReduction)

    /** 物理抗性（0..1） */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalResistance)
    FGameplayAttributeData PhysicalResistance;
    ATTRIBUTE_ACCESSORS(UAbyssAttributeSet, PhysicalResistance)

    /** 法术抗性（0..1） */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicResistance)
    FGameplayAttributeData MagicResistance;
    ATTRIBUTE_ACCESSORS(UAbyssAttributeSet, MagicResistance)
	
	/** 技能点 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SkillPoints)
	FGameplayAttributeData SkillPoints;
	ATTRIBUTE_ACCESSORS(ThisClass, SkillPoints);
	
	/** 天赋点 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TalentPoints)
	FGameplayAttributeData TalentPoints;
	ATTRIBUTE_ACCESSORS(ThisClass, TalentPoints);
	
	/** 技能栏数量 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SkillSlotAmount)
	FGameplayAttributeData SkillSlotAmount;
	ATTRIBUTE_ACCESSORS(ThisClass, SkillSlotAmount);
	
	// -------------------------
	// 等级系统
	// -------------------------
	/** 当前累计的总经验值 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentXP)
	FGameplayAttributeData CurrentXP;
	ATTRIBUTE_ACCESSORS(ThisClass, CurrentXP);
	
	/** 当前等级 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Level)
	FGameplayAttributeData Level;
	ATTRIBUTE_ACCESSORS(ThisClass, Level);
	
	/** 经验倍率 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_XPMultiplier)
	FGameplayAttributeData XPMultiplier;
	ATTRIBUTE_ACCESSORS(ThisClass, XPMultiplier);
	
	// -------------------------
	// Meta 元属性
	// -------------------------
	UPROPERTY(BlueprintReadOnly,Category = "Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UAbyssAttributeSet, IncomingDamage);
	
	UPROPERTY(BlueprintReadOnly,Category = "Meta Attributes")
	FGameplayAttributeData IncomingXP;
	ATTRIBUTE_ACCESSORS(ThisClass, IncomingXP);

	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_PhysicalPower(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MagicPower(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Defense(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_CritRate(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_CritDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MagicPenetration(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_PhysicalPenetration(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_AttackSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_CooldownReduction(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_PhysicalResistance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MagicResistance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_SkillPoints(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_TalentPoints(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_SkillSlotAmount(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_CurrentXP(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_Level(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_XPMultiplier(const FGameplayAttributeData& OldValue);
};
