// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbyssTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbyssBlueprintLibrary.generated.h"

class UAbyssCombatComponentBase;

UENUM(BlueprintType)
enum class EHitDirection : uint8
{
	Left,
	Right,
	Forward,
	Back
};

USTRUCT(BlueprintType)
struct FClosestActorWithTagResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> Actor;

	UPROPERTY(BlueprintReadWrite)
	float Distance{0.f};
};

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure)
	static EHitDirection GetHitDirection(const FVector& TargetForward, const FVector& ToInstigator);

	UFUNCTION(BlueprintPure)
	static FName GetHitDirectionName(const EHitDirection& HitDirection);

	static UAbyssCombatComponentBase* NativeGetCombatComponentFromActor(AActor* InActor);
	
	UFUNCTION(BlueprintCallable,Category = "Abyss|FunctionLibrary",meta = (DisplayName="Get Combat Component From Actor",ExpandEnumAsExecs = "OutValidType"))
	static UAbyssCombatComponentBase* BP_GetCombatComponentFromActor(AActor* InActor,EAbyssValidType& OutValidType);
};
