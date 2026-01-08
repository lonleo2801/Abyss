// Copyright (c) 2025 Leon Lee


#include "AbyssAnimInstanceCharacter.h"
#include "Camera/CameraComponent.h"
#include "Characters/Hero/Character/AbyssHeroBase.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"

UAbyssAnimInstanceCharacter::UAbyssAnimInstanceCharacter()
{
	MeshAndActorOffset = FRotator::ZeroRotator;
	MeshAndActorOffset.Yaw = -90.f;

	bDoOnceDelaySpeed = false;

	RotationModeType = ECharacterRotationModeType::Strafe;
	GaitLastFrame = ECharacterGaitType::Gait_Walk;

	HeavyLandSpeedThreshold = 700.f;

	bJustLanded = false;
}

void UAbyssAnimInstanceCharacter::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	HeroCharacter = Cast<AAbyssHeroBase>(TryGetPawnOwner());
	if (HeroCharacter.IsValid())
	{
		GaitType = HeroCharacter->GetGaitType();
		GaitLastFrame = GaitType;
	}
}

void UAbyssAnimInstanceCharacter::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	//获取根骨是否开启偏移
	bOffsetRootBoneEnabled = UKismetSystemLibrary::GetConsoleVariableBoolValue(TEXT("a.animnode.offsetrootbone.enable"));

	GenerateTrajectory(DeltaSeconds);

	if (HeroCharacter.IsValid())
	{
		GaitLastFrame = GaitType;
		GaitType = HeroCharacter->GetGaitType();

		bJustLanded = HeroCharacter->IsJustLanded();
		LandVelocity = HeroCharacter->GetLandVelocity();

		if (UCharacterMovementComponent* InCharacterMovement = HeroCharacter->GetCharacterMovement())
		{
			RotationModeType =
				InCharacterMovement->bOrientRotationToMovement ?
				ECharacterRotationModeType::OrientToMovement :
				ECharacterRotationModeType::Strafe;
		}

		if (Speed == 0.f && bDoOnceDelaySpeed)
		{
			bDoOnceDelaySpeed = false;

			bDelaySpeed = true;
			bDelaySpeed = 1.f;
		}

		if (Speed > 0.f)
		{
			bDoOnceDelaySpeed = true;
			bDelaySpeed = true;
			bDelaySpeed = 1.f;
		}
	}

	bDelaySpeed.Tick(DeltaSeconds);
}

void UAbyssAnimInstanceCharacter::GenerateTrajectory(float DeltaSeconds)
{
	FTransformTrajectory OutTrajectory;
	UPoseSearchTrajectoryLibrary::PoseSearchGenerateTransformTrajectory(
		GetOwningComponent()->GetAnimInstance(),
		Speed > 0.f ? TrajectoryGenerationData_Moving : TrajectoryGenerationData_Idle,
		GetDeltaSeconds(),
		QueryTrajectory,
		PreviousDesiredControllerYaw,
		OutTrajectory,
		-1.f,
		30,
		0.1f,
		15);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwningActor());

	FPoseSearchTrajectory_WorldCollisionResults OutCollisionResult;
	UPoseSearchTrajectoryLibrary::HandleTransformTrajectoryWorldCollisions(
		GetWorld(),
		this,
		OutTrajectory,
		true,
		0.01f,
		QueryTrajectory,
		OutCollisionResult,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		/*bDrawDebug ? EDrawDebugTrace::ForDuration :*/ EDrawDebugTrace::None,
		true,
		150.f);

	//采样0.5f和0.4f秒的对应的内容
	FTransformTrajectorySample OutATrajectorySample;
	FTransformTrajectorySample OutBTrajectorySample;
	UPoseSearchTrajectoryLibrary::GetTransformTrajectorySampleAtTime(QueryTrajectory, 0.5f, OutATrajectorySample, false);
	UPoseSearchTrajectoryLibrary::GetTransformTrajectorySampleAtTime(QueryTrajectory, 0.4f, OutBTrajectorySample, false);

	//未来的速度
	FutureVelocity = (OutATrajectorySample.Position - OutBTrajectorySample.Position) * 10.f;
}

bool UAbyssAnimInstanceCharacter::IsStarting() const
{
	return FutureVelocity.Size2D() >= SpeedDirection.Size2D() + 100.f && IsMoving();
}

bool UAbyssAnimInstanceCharacter::ShouldTurnInPlace() const
{
	if (HeroCharacter.IsValid())
	{
		FRotator AnimRotation = HeroCharacter->GetFollowCamera()->GetComponentRotation();
		AnimRotation += MeshAndActorOffset;

		FRotator CharacterRotation = HeroCharacter->GetMesh()->GetComponentRotation();

		FRotator RotationDelta = UKismetMathLibrary::NormalizedDeltaRotator(
			AnimRotation,
			CharacterRotation);

		if (IsDebug() && IsInGameThread())//保证要在主线程
		{
			FVector AnimVector = AnimRotation.Vector();
			FVector CharacterVector = CharacterRotation.Vector();

			FVector InCenterPoint = HeroCharacter->GetActorLocation();

			InCenterPoint.Z += 50.f;
			FVector AnimPosition = AnimVector * 200.f + InCenterPoint;

			DrawDebugString(GetWorld(), AnimPosition, RotationDelta.ToString(), nullptr, FColor::Red, 0.1f);

			//显示本身的forward方向
			DrawDebugDirectionalArrow(GetWorld(), InCenterPoint, AnimPosition, 2.f, FColor::Red, false, 0.1f);

			//显示本身的Right方向
			DrawDebugDirectionalArrow(GetWorld(), InCenterPoint, CharacterVector * 200.f + InCenterPoint, 2.f, FColor::Green, false, 0.1f);
		}

		return FMath::Abs(RotationDelta.Yaw) > 35.f;
	}

	return false;
}

bool UAbyssAnimInstanceCharacter::IsDelaySpeed() const
{
	return *bDelaySpeed;
}

bool UAbyssAnimInstanceCharacter::IsPivot() const
{
	FRotator RotationDelta = UKismetMathLibrary::NormalizedDeltaRotator(
		FutureVelocity.Rotation(),
		SpeedDirection.Rotation());

	return RotationDelta.Yaw > 50.f;
}

EOffsetRootBoneMode UAbyssAnimInstanceCharacter::GetOffsetRootRotationMode() const
{
	return IsSlotActive(TEXT("DefaultSlot")) ? EOffsetRootBoneMode::Release : EOffsetRootBoneMode::Accumulate;
}

EOffsetRootBoneMode UAbyssAnimInstanceCharacter::GetOffsetRootTranslationMode() const
{
	if (IsSlotActive(TEXT("DefaultSlot")))
	{
		return EOffsetRootBoneMode::Release;
	}

	if (bInAir)
	{
		return EOffsetRootBoneMode::Release;
	}

	if (!IsMoving())
	{
		return EOffsetRootBoneMode::Release;
	}

	return EOffsetRootBoneMode::Interpolate;
}

float UAbyssAnimInstanceCharacter::GetOffsetRootTranslationHalfLife() const
{
	return IsMoving() ? 0.2f : 0.1f;
}

float UAbyssAnimInstanceCharacter::GetMMBlendTime()
{
	switch (MovementModeType)
	{
	case ECharacterMovementModeType::Movement_OnGround:
		{
			switch (MovementModeLastFrame)
			{
			case ECharacterMovementModeType::Movement_OnGround:
				{
					return 0.4f;
				}
			case ECharacterMovementModeType::Movement_InAir:
				{
					return 0.2f;
				}
			}
			break;
		}
	case ECharacterMovementModeType::Movement_InAir:
		{
			if (Velocity.Z > 100)
			{
				return 0.15f;
			}

			return 0.5f;
		}
	}

	return 0.0f;
}

EPoseSearchInterruptMode UAbyssAnimInstanceCharacter::GetMMInterruptMode()
{
	if (MovementModeType != MovementModeLastFrame &&
		((MovementStateType != MovementStateLastFrame ||
			GaitType != GaitLastFrame ||
			StandType != StandLastFrame) && MovementModeType == ECharacterMovementModeType::Movement_OnGround))
	{
		return EPoseSearchInterruptMode::DoNotInterrupt;
	}

	return EPoseSearchInterruptMode::InterruptOnDatabaseChange;
}

FQuat UAbyssAnimInstanceCharacter::GetDesiredFacing()
{
	FTransformTrajectorySample OutATrajectorySample;
	UPoseSearchTrajectoryLibrary::GetTransformTrajectorySampleAtTime(QueryTrajectory, 0.5f, OutATrajectorySample, false);

	return OutATrajectorySample.Facing;
}

FVector2D UAbyssAnimInstanceCharacter::GetLeanAmount() const
{
	FVector MyCalcuateValue = CalculateRelativeAccelerationAmount();

	auto MyValue = MyCalcuateValue.Y * FMath::GetMappedRangeValueClamped(
		FVector2D(200.f, 500.f),
		FVector2D(0.5f, 1.f), Speed);

	return FVector2D(MyValue, 0.f);
}

bool UAbyssAnimInstanceCharacter::EnableSteering() const
{
	return  MovementStateType == ECharacterMovementStateType::Movement_Moving ||
		  MovementModeType != ECharacterMovementModeType::Movement_InAir;
}

EOrientationWarpingSpace UAbyssAnimInstanceCharacter::GetOrientationWarpingWarpingSpace() const
{
	return bOffsetRootBoneEnabled ? EOrientationWarpingSpace::RootBoneTransform : EOrientationWarpingSpace::ComponentTransform;
}

bool UAbyssAnimInstanceCharacter::IsJustLandedLight() const
{
	return bJustLanded && FMath::Abs(LandVelocity.Z) < HeavyLandSpeedThreshold;
}

bool UAbyssAnimInstanceCharacter::IsJustLandedHeavy() const
{
	return bJustLanded && FMath::Abs(LandVelocity.Z) >= HeavyLandSpeedThreshold;
}

bool UAbyssAnimInstanceCharacter::IsJustTraversed() const
{
	return !IsSlotActive(TEXT("DefaultSlot")) && GetCurveValue(TEXT("MovingTraversal")) > 0;
}
