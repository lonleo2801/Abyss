// Copyright (c) 2025 Leon Lee


#include "UAbyssDamageGameplayEffectExecCalc.h"

#include "AbyssDebugHelper.h"
#include "AbilitySystem/Attributes/AbyssAttributeSet.h"
#include "AbilitySystem/BlueprintLibrary/AbyssAbilitySystemLibrary.h"
#include "AbilitySystem/BlueprintLibrary/AbyssEffectLibrary.h"
#include "Data/AbyssDataRegistryManager.h"
#include "Data/RegistryTypeDefine.h"
#include "Data/DataTables/AbyssDamageTypeInfoRow.h"
#include "GameplayTags/AbyssTags.h"
#include "Interfaces/AbyssCombatInterface.h"

struct AbyssDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalPower);
    DECLARE_ATTRIBUTE_CAPTUREDEF(MagicPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defense);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CritRate);  
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(MagicResistance);
	
	AbyssDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAbyssAttributeSet, PhysicalPower, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAbyssAttributeSet, MagicPower, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAbyssAttributeSet, Defense, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAbyssAttributeSet, CritRate, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAbyssAttributeSet, CritDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAbyssAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAbyssAttributeSet, MagicPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAbyssAttributeSet, PhysicalPenetration, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAbyssAttributeSet, PhysicalResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAbyssAttributeSet, MagicResistance, Target, false);

	}
};
static const AbyssDamageStatics& DamageStatics()
{
	static AbyssDamageStatics DStatics;
	return DStatics;
}

UUAbyssDamageGameplayEffectExecCalc::UUAbyssDamageGameplayEffectExecCalc()
{
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().DefenseDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritRateDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicResistanceDef);
}

void UUAbyssDamageGameplayEffectExecCalc::DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
	const FGameplayEffectSpec& Spec,FAggregatorEvaluateParameters EvaluationParameters) const
{
	for (TTuple<FGameplayTag,FGameplayTag> Pair : UAbyssAbilitySystemLibrary::GetPlayerAbilityRuntimeData(ExecutionParams.GetSourceAbilitySystemComponent())->DamageTypesToDebuffMap)
	{
		const FGameplayTag& DamageTypeTag = Pair.Key;
		const FGameplayTag& DebuffTypeTag = Pair.Value;
		const float TypeDamage =  Spec.GetSetByCallerMagnitude(DamageTypeTag, false , -1.f);
		if (TypeDamage > -.5f)
		{
			// 判断Debuff有没有成功生效
			const float DebuffSuccessChance =  Spec.GetSetByCallerMagnitude(AbyssTags::SetByCaller::Debuff::Chance ,  false , -1.f);
			//TODO: 可以加上目标韧性判断
			const bool bDebuffSuccess = FMath::RandRange(1, 100) < DebuffSuccessChance;
			if (bDebuffSuccess)
			{
				FGameplayEffectContextHandle ContextHandle = Spec.GetContext();
				UAbyssEffectLibrary::SetIsSuccessfulDebuff(ContextHandle,bDebuffSuccess);
				UAbyssEffectLibrary::SetDamageType(ContextHandle,DamageTypeTag);
				
				const float DebuffDamage = Spec.GetSetByCallerMagnitude(AbyssTags::SetByCaller::Debuff::Damage,false,-1.f);
				const float DebuffDuration = Spec.GetSetByCallerMagnitude(AbyssTags::SetByCaller::Debuff::Duration,false,-1.f);
				const float DebuffFrequency = Spec.GetSetByCallerMagnitude(AbyssTags::SetByCaller::Debuff::Frequency,false,-1.f);
				
				UAbyssEffectLibrary::SetDebuffDamage(ContextHandle,DebuffDamage);
				UAbyssEffectLibrary::SetDebuffDuration(ContextHandle,DebuffDuration);
				UAbyssEffectLibrary::SetDebuffFrequency(ContextHandle,DebuffFrequency);
			}
		}
	}
}


void UUAbyssDamageGameplayEffectExecCalc::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

    AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
    IAbyssCombatInterface* SourceCombatInterface = Cast<IAbyssCombatInterface>(SourceAvatar);
    IAbyssCombatInterface* TargetCombatInterface = Cast<IAbyssCombatInterface>(TargetAvatar);

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;
	
	//Debuff
	DetermineDebuff(ExecutionParams,Spec,EvaluationParameters);
	

    // Get Damage Set by Caller Magnitude
	float Damage = 0;
	UAbyssDataRegistryManager::ForEachCached<FAbyssDamageTypeInfoRow>(AbyssRegistryType::DamageTypeInfo,[&Damage,Spec](const FName& Name, const FAbyssDamageTypeInfoRow& Row)
	{
		const float DamageTypeValue = Spec.GetSetByCallerMagnitude(Row.AttributeTag ,  false);
		Damage += DamageTypeValue;
	});
      

	// ***************
	// 格挡几率
	// **************  
    float TargetBlockChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
    TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);

    const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	
	UAbyssEffectLibrary::SetIsBlockedHit(EffectContextHandle,bBlocked);

    // If Block, no damage.
	if (bBlocked)
	{
		Damage = 1;
		const FGameplayModifierEvaluatedData EvaluatedData(UAbyssAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
		OutExecutionOutput.AddOutputModifier(EvaluatedData);
		return;
	}

	// ***************
	// 物理穿甲和伤害减免
	// **************
	//伤害减免百分比
    float TargetDamageReduce= 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseDef, EvaluationParameters, TargetDamageReduce);
    TargetDamageReduce = FMath::Max<float>(TargetDamageReduce, 0.f);

	//物理穿透
    float SourcePhysicalPenetration = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PhysicalPenetrationDef, EvaluationParameters, SourcePhysicalPenetration);
    SourcePhysicalPenetration = FMath::Max<float>(SourcePhysicalPenetration, 0.f);

	//物理防御
	float TargetPhysicalResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PhysicalResistanceDef, EvaluationParameters, TargetPhysicalResistance);
	TargetPhysicalResistance = FMath::Max<float>(TargetPhysicalResistance, 0.f);

    // 物理穿透无视一部分物理防御
    const float EffectivePhysicalResistance = TargetPhysicalResistance * ( 100 - SourcePhysicalPenetration ) / 100.f;

    //TODO:	从数据注册表找出物理防御减免多少伤害的曲线,要新建一种数据注册表，存储各种属性影响数值的曲线，比如横坐标是护甲，纵坐标是伤害减免百分比
    /*const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
    const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
    // Armor ignores a percentage of incoming Damage.
    Damage *= ( 100 - EffectiveArmor * EffectiveArmorCoefficient ) / 100.f;*/

	Damage = Damage * (100-TargetDamageReduce)/100.f;
    

	// ***************
	// Critical Hit
	// **************
    float SourceCriticalHitChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritRateDef, EvaluationParameters, SourceCriticalHitChance);
    SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);

    float SourceCriticalHitDamage = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritDamageDef, EvaluationParameters, SourceCriticalHitDamage);
    SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.f);

    const bool bCriticalHit = FMath::RandRange(1, 100) < SourceCriticalHitChance;
	UAbyssEffectLibrary::SetIsCriticalHit(EffectContextHandle,bCriticalHit);

    Damage = bCriticalHit ? SourceCriticalHitDamage * Damage : Damage;
	
	//最终确认伤害数值	
    const FGameplayModifierEvaluatedData EvaluatedData(UAbyssAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
    OutExecutionOutput.AddOutputModifier(EvaluatedData);
	
}
