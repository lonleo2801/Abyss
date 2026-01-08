// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AbyssAIController.generated.h"

struct FAIStimulus;
class UAISenseConfig_Sight;

UCLASS()
class ABYSS_API AAbyssAIController : public AAIController
{
	GENERATED_BODY()

public:
	AAbyssAIController(const FObjectInitializer& ObjectInitializer);

	//~ Begin IGenericTeamAgentInterface Interface
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	//~ End IGenericTeamAgentInterface Interface
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UAIPerceptionComponent> EnemyPerceptionComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UAISenseConfig_Sight> AISenseConfig_Sight;

	UFUNCTION()
	virtual void OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
	UPROPERTY(EditDefaultsOnly,Category = "Detour Crowd Avoidance Config")
	bool bEnableDetourCrowdAvoidance = true;
	
	UPROPERTY(EditDefaultsOnly,Category = "Detour Crowd Avoidance Config",meta=(EditCondition="bEnableDetourCrowdAvoidance",UIMin = "1",UIMax = "4"))
	int32 DetourCrowdAvoidanceQuality = 4;

	UPROPERTY(EditDefaultsOnly,Category = "Detour Crowd Avoidance Config",meta=(EditCondition="bEnableDetourCrowdAvoidance"))
	float CollisionQueryRange = 600.f;
};
