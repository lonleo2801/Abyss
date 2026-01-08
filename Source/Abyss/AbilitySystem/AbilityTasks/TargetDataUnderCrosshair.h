// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderCrosshair.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCrosshairTargetDataSignature, const FGameplayAbilityTargetDataHandle &, DataHandle);
/**
 * 
 */
UCLASS()
class ABYSS_API UTargetDataUnderCrosshair : public UAbilityTask
{
	GENERATED_BODY()
public:
	// 修改工厂函数，增加 bRepeat 和 TimeInterval 参数
	UFUNCTION(BlueprintCallable, Category = "Abyss|Ability|Tasks", 
		meta = (DisplayName = "TargetDataUnderCrosshair",
			HidePin = "OwningAbility",
			DefaultToSelf = "OwningAbility",
			BlueprintInternalUseOnly = "true"))
	static UTargetDataUnderCrosshair* CreateTargetDataUnderCrosshair(UGameplayAbility* OwningAbility, bool bRepeat = false, float Interval = 0.1f);

	UPROPERTY(BlueprintAssignable)
	FCrosshairTargetDataSignature ValidData;

private:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override; // 新增：清理Timer

	void SendCrosshairTargetData();
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);

	// 新增变量
	bool bShouldRepeat;
	float ReportInterval;
	FTimerHandle TimerHandle_Trace;
};
