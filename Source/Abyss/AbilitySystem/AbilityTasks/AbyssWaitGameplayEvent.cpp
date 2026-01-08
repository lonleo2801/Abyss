// Copyright (c) 2025 Leon Lee


#include "AbyssWaitGameplayEvent.h"

UAbyssWaitGameplayEvent* UAbyssWaitGameplayEvent::WaitGameplayEventToActorProxy(AActor* TargetActor,
	FGameplayTag EventTag, bool OnlyTriggerOnce, bool OnlyMatchExact)
{
	UAbyssWaitGameplayEvent* MyObj = NewObject<UAbyssWaitGameplayEvent>();
	MyObj->SetAbilityActor(TargetActor);
	MyObj->Tag = EventTag;
	MyObj->OnlyTriggerOnce = OnlyTriggerOnce;
	MyObj->OnlyMatchExact = OnlyMatchExact;
	return MyObj;
}

void UAbyssWaitGameplayEvent::StartActivation()
{
	Activate();
}
