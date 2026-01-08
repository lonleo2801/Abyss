// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "CommonGameInstance.h"
#include "AbyssGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()

public:
	UAbyssGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Init() override;
	virtual void Shutdown() override;
};
