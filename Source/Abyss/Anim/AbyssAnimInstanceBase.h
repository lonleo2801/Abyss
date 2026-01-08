// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbyssTypes.h"
#include "Animation/AnimInstance.h"
#include "AbyssAnimInstanceBase.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()
public:
	UAbyssAnimInstanceBase();

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "AnimAttrubute")
	bool bDrawDebug;

public:
	void UpdateSpeed(float DeltaSeconds);

	UFUNCTION(BlueprintPure, Category = "AnimInstance|StepMatching", meta = (BlueprintThreadSafe))
	FVector CalculateRelativeAccelerationAmount() const;

public:
	UFUNCTION(BlueprintPure, Category = "AnimInstance|StepMatching", meta = (BlueprintThreadSafe))
	virtual FVector GetActorForwardVector() const;

	UFUNCTION(BlueprintPure, Category = "AnimInstance|StepMatching", meta = (BlueprintThreadSafe))
	virtual FRotator GetActorRotation() const;

	UFUNCTION(BlueprintPure, Category = "AnimInstance|StepMatching", meta = (BlueprintThreadSafe))
	virtual FTransform GetActorTransform() const;

public:
	UFUNCTION(BlueprintPure, Category = "AnimInstance|Move", meta = (BlueprintThreadSafe))
	bool IsMoving() const;

	UPROPERTY(Category = ShooterVRCharacter, BlueprintReadOnly)
	FTransform CharacterTransform;

	UPROPERTY(Category = ShooterVRCharacter, BlueprintReadOnly)
	FVector SpeedDirection;

	UPROPERTY(Category = ShooterVRCharacter, BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(Category = ShooterVRCharacter, BlueprintReadOnly)
	FVector Acceleration;

	UPROPERTY(Category = ShooterVRCharacter, BlueprintReadOnly)
	FVector VelocityAcceleration;

	UPROPERTY(Category = ShooterVRCharacter, BlueprintReadOnly)
	FVector VelocityLastFrame;

	UPROPERTY(Category = ShooterVRCharacter, BlueprintReadOnly)
	FVector LastNonZeroVelocity;

	UPROPERTY(Category = ShooterVRCharacter, BlueprintReadOnly)
	FRotator ControllerRotator;

	UPROPERTY(Category = ShooterVRCharacter, BlueprintReadOnly)
	float Speed;

	UPROPERTY(Category = ShooterVRCharacter, BlueprintReadOnly)
	bool bAcceleration;

	UPROPERTY(Category = ShooterVRCharacter, BlueprintReadOnly)
	bool bHasVelocity;

	UPROPERTY(Category = ShooterVRCharacter, BlueprintReadOnly)
	bool bInAir;

	UPROPERTY(Category = ShooterVRCharacter, BlueprintReadOnly)
	bool bDie;

	UPROPERTY(Category = ShooterVRCharacter, BlueprintReadOnly)
	bool bCrouch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	ECharacterMovementModeType MovementModeType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	ECharacterMovementModeType MovementModeLastFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	ECharacterMovementStateType MovementStateType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	ECharacterMovementStateType MovementStateLastFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	ECharacterStandType StandType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	ECharacterStandType StandLastFrame;

	bool IsDebug() const { return bDrawDebug; }
	
};
