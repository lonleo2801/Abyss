// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Components/AbyssExtensionComponentBase.h"
#include "AbyssCombatComponentBase.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ABYSS_API UAbyssCombatComponentBase : public UAbyssExtensionComponentBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Abyss|MotionWarping")
	void SetFacingTarget(const FVector& FacingVector);

	UFUNCTION(BlueprintCallable, Category = "Abyss|MotionWarping")
	void SetFacingToCameraDirection();

	
	
private:
	bool bDead = false;
};
