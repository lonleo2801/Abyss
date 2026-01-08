// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Characters/Enemy/AbyssEnemyBase.h"

#include "AbyssEnemyMinions.generated.h"

UCLASS()
class ABYSS_API AAbyssEnemyMinions : public AAbyssEnemyBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAbyssEnemyMinions();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
