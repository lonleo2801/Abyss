// Copyright (c) 2025 Leon Lee


#include "AbyssEnemyMinions.h"


// Sets default values
AAbyssEnemyMinions::AAbyssEnemyMinions()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAbyssEnemyMinions::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAbyssEnemyMinions::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AAbyssEnemyMinions::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

