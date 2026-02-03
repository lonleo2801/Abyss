// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"


USTRUCT(BlueprintType)
struct FInteractionData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ActionText; // 例如 "拾取"

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InteractionDuration = 0.0f; // 0=瞬间，>0=长按
};

// This class does not need to be modified.
UINTERFACE()
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ABYSS_API IInteractableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 获取交互提示数据 (Client)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FInteractionData GetInteractionData();

	// 触发交互 (Server)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool TriggerInteraction(APawn* Interactor);
};
