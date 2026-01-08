// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbyssHeroBase.h"
#include "Logging/LogMacros.h"
#include "AbyssHeroWarrior.generated.h"

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AAbyssHeroWarrior : public AAbyssHeroBase 
{
	GENERATED_BODY()
public:
	/** Constructor */
	AAbyssHeroWarrior();
	
};

