// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "AbyssViewModel_GameMenuInteraction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonPressed);
/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssViewModel_GameMenuInteraction : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
private:
	
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	FText Slot1;

public:
	
	UPROPERTY()
	FOnButtonPressed OnEquipSkill1Pressed;
	
	UFUNCTION(BlueprintCallable)
	void SetEquipSkill1()
	{
		OnEquipSkill1Pressed.Broadcast();
	}
	
	FText GetSlot1() const { return Slot1; }
	
	void SetSlot1(FText NewSlot1)
	{
		UE_MVVM_SET_PROPERTY_VALUE(Slot1, NewSlot1);
	}
};
