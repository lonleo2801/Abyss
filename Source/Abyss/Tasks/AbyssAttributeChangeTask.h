// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AbyssAttributeChangeTask.generated.h"

struct FOnAttributeChangeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChanged, FGameplayAttribute,Attribute,float,NewValue,float,OldValue);

/**
 * 
 */
UCLASS(BlueprintType,meta=(ExposedAsyncProxy = AsyncTask))
class ABYSS_API UAbyssAttributeChangeTask : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnAttributeChanged;

	UFUNCTION(BlueprintCallable,meta= (BlueprintInternalUseOnly = "true"))
	static UAbyssAttributeChangeTask* ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute);

	UFUNCTION(BlueprintCallable)
	void EndTask();

	TWeakObjectPtr<UAbilitySystemComponent> ASC;
	FGameplayAttribute AttributeToListenFor;

	
	void AttributeChanged(const FOnAttributeChangeData& Data);
};
















