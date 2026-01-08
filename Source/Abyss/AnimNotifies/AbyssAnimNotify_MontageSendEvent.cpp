// Copyright (c) 2025 Leon Lee


#include "AbyssAnimNotify_MontageSendEvent.h"

#include "AbilitySystem/BlueprintLibrary/AbyssAbilitySystemLibrary.h"


void UAbyssAnimNotify_MontageSendEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	FGameplayEventData EventPayload;
	EventPayload.Instigator = MeshComp->GetOwner();
	
	UAbyssAbilitySystemLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, EventPayload);
}
