// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "AbyssPickupActor.generated.h"

class UAbyssActivatableWidget;
class USphereComponent;

UCLASS()
class ABYSS_API AAbyssPickupActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAbyssPickupActor();
	
	void OpenPickupMeshSimulatePhysics(bool bOpenPhysics);

	// 拾取碰撞盒
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> PickupSphere;

	// 模型
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> PickupMesh;
	
	UPROPERTY(EditAnywhere, Category = "Abyss|Pickup")
	FText PickupText = FText::FromString("Press F To Pickup");
	
	UPROPERTY(EditAnywhere, Category = "Abyss|Pickup")
	float InteractionDuration = 0.0f;
	
	// --- Interface ---
	virtual FInteractionData GetInteractionData_Implementation() override;
};
