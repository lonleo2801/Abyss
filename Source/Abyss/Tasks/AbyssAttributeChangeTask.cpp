// Copyright (c) 2025 Leon Lee


#include "AbyssAttributeChangeTask.h"
#include "AbilitySystemComponent.h" 


UAbyssAttributeChangeTask* UAbyssAttributeChangeTask::ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute)
{
	UAbyssAttributeChangeTask* WaitForAttributeChangeTask = NewObject<UAbyssAttributeChangeTask>();
	WaitForAttributeChangeTask->ASC = AbilitySystemComponent;
	WaitForAttributeChangeTask->AttributeToListenFor = Attribute;

	if (!IsValid(AbilitySystemComponent))
	{
		WaitForAttributeChangeTask->RemoveFromRoot();
		return nullptr;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(WaitForAttributeChangeTask, &UAbyssAttributeChangeTask::AttributeChanged);
	return WaitForAttributeChangeTask;
}

void UAbyssAttributeChangeTask::EndTask()
{
	if (ASC.IsValid())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).RemoveAll(this);
	}

	SetReadyToDestroy();
	MarkAsGarbage();
}

void UAbyssAttributeChangeTask::AttributeChanged(const FOnAttributeChangeData& Data)
{
	OnAttributeChanged.Broadcast(Data.Attribute,Data.NewValue,Data.OldValue);
}


