// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbyssHeroBase.h"
#include "AbyssHeroWithWeapon.generated.h"

UCLASS()
class ABYSS_API AAbyssHeroWithWeapon : public AAbyssHeroBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAbyssHeroWithWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
	
};
