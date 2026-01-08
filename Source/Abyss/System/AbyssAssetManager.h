// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AbyssAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:

	static UAbyssAssetManager& Get();

protected:

	virtual void StartInitialLoading() override;
};
