// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/** Main log category used across the project */
DECLARE_LOG_CATEGORY_EXTERN(LogAbyss, Log, All);

#define ECC_Projectile ECollisionChannel::ECC_GameTraceChannel1
#define ECC_CrosshairTrace ECollisionChannel::ECC_GameTraceChannel2