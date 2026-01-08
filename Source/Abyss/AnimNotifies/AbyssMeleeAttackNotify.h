// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AbyssMeleeAttackNotify.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssMeleeAttackNotify : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

private:

	UPROPERTY(EditAnywhere, Category = "Abyss|Debugs")
	bool bDrawDebugs = true;

	UPROPERTY(EditAnywhere, Category = "Abyss|Socket")
	FName SocketName{"FX_Trail_01_R"};

	UPROPERTY(EditAnywhere, Category = "Abyss|Socket")
	float SocketExtensionOffset{40.f};

	UPROPERTY(EditAnywhere, Category = "Abyss|Socket")
	float SphereTraceRadius{60.f};

	TArray<FHitResult> PerformSphereTrace(USkeletalMeshComponent* MeshComp) const;
	void SendEventsToActors(USkeletalMeshComponent* MeshComp, const TArray<FHitResult>& Hits) const;
};
