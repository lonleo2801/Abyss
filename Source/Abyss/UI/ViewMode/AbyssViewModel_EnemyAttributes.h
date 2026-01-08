// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "AbyssViewModel_EnemyAttributes.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssViewModel_EnemyAttributes : public UMVVMViewModelBase
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float Health;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	float MaxHealth;
public:
	UFUNCTION(BlueprintPure, FieldNotify)
	float GetHealthPercent() const { return MaxHealth > 0.f ? Health / MaxHealth : 0.f; }
	float GetHealth() const { return Health; }
	float GetMaxHealth() const { return MaxHealth; }
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
};
