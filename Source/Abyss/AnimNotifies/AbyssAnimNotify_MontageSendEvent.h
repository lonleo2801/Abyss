// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AbyssAnimNotify_MontageSendEvent.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssAnimNotify_MontageSendEvent : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere)
	FGameplayTag EventTag;
};
