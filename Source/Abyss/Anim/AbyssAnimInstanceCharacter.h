// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbyssAnimInstanceBase.h"
#include "BoneControllers/AnimNode_OffsetRootBone.h"
#include "BoneControllers/AnimNode_OrientationWarping.h"
#include "PoseSearch/PoseSearchLibrary.h"
#include "PoseSearch/PoseSearchTrajectoryLibrary.h"
#include "AbyssAnimInstanceCharacter.generated.h"

class AAbyssHeroBase;
/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssAnimInstanceCharacter : public UAbyssAnimInstanceBase
{
	GENERATED_BODY()

public:
	UAbyssAnimInstanceCharacter();
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	FPoseSearchTrajectoryData TrajectoryGenerationData_Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	FPoseSearchTrajectoryData TrajectoryGenerationData_Moving;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	FTransformTrajectory QueryTrajectory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	float PreviousDesiredControllerYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	FVector FutureVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	ECharacterGaitType GaitType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	ECharacterGaitType GaitLastFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	ECharacterRotationModeType RotationModeType;

	UPROPERTY(Category = ShooterVRCharacter, BlueprintReadOnly)
	FRotator MeshAndActorOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	bool bOffsetRootBoneEnabled;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	float HeavyLandSpeedThreshold;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	bool bJustLanded;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	FVector LandVelocity;

protected:
	void GenerateTrajectory(float DeltaSeconds);

public:
	UFUNCTION(BlueprintPure, Category = "AnimInstance|Move", meta = (BlueprintThreadSafe))
	bool IsStarting() const;

	UFUNCTION(BlueprintPure, Category = "AnimInstance|Move", meta = (BlueprintThreadSafe))
	bool ShouldTurnInPlace() const;

	UFUNCTION(BlueprintPure, Category = "AnimInstance|Move", meta = (BlueprintThreadSafe))
	bool IsDelaySpeed() const;

	UFUNCTION(BlueprintPure, Category = "AnimInstance|Move", meta = (BlueprintThreadSafe))
	bool IsPivot() const;

	UFUNCTION(BlueprintPure, Category = "AnimInstance|OffsetRoot", meta = (BlueprintThreadSafe))
	EOffsetRootBoneMode GetOffsetRootRotationMode() const;

	UFUNCTION(BlueprintPure, Category = "AnimInstance|OffsetRoot", meta = (BlueprintThreadSafe))
	EOffsetRootBoneMode GetOffsetRootTranslationMode() const;

	UFUNCTION(BlueprintPure, Category = "AnimInstance|OffsetRoot", meta = (BlueprintThreadSafe))
	float GetOffsetRootTranslationHalfLife() const;

	UFUNCTION(BlueprintPure, Category = "AnimInstance|OffsetRoot", meta = (BlueprintThreadSafe))
	float GetMMBlendTime();

	UFUNCTION(BlueprintPure, Category = "AnimInstance|OffsetRoot", meta = (BlueprintThreadSafe))
	EPoseSearchInterruptMode GetMMInterruptMode();

	UFUNCTION(BlueprintPure, Category = "AnimInstance|OffsetRoot", meta = (BlueprintThreadSafe))
	FQuat GetDesiredFacing();

public:
	UFUNCTION(BlueprintPure, Category = "AnimInstance|OffsetRoot", meta = (BlueprintThreadSafe))
	FVector2D GetLeanAmount() const;

	UFUNCTION(BlueprintPure, Category = "AnimInstance|OffsetRoot", meta = (BlueprintThreadSafe))
	bool EnableSteering() const;

	UFUNCTION(BlueprintPure, Category = "AnimInstance|OffsetRoot", meta = (BlueprintThreadSafe))
	EOrientationWarpingSpace GetOrientationWarpingWarpingSpace() const;

public:
	UFUNCTION(BlueprintPure, Category = "AnimInstance|OffsetRoot", meta = (BlueprintThreadSafe))
	bool IsJustLandedLight() const;

	UFUNCTION(BlueprintPure, Category = "AnimInstance|OffsetRoot", meta = (BlueprintThreadSafe))
	bool IsJustLandedHeavy() const;

	UFUNCTION(BlueprintPure, Category = "AnimInstance|OffsetRoot", meta = (BlueprintThreadSafe))
	bool IsJustTraversed() const;



protected:
	UPROPERTY()
	TWeakObjectPtr<AAbyssHeroBase> HeroCharacter;

	FResetBool bDelaySpeed;

	UPROPERTY()
	bool bDoOnceDelaySpeed;	
};
