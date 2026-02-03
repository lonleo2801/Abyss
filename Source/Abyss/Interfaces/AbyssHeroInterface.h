// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AbyssHeroInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UAbyssHeroInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ABYSS_API IAbyssHeroInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	virtual void LevelUp(int32 NewLevel) =0;
};
