// Copyright (c) 2025 Leon Lee


#include "AbyssAnimInstanceBase.h"

#include "Characters/Hero/Character/AbyssHeroBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


UAbyssAnimInstanceBase::UAbyssAnimInstanceBase()
{
	bCrouch = false;
	Speed = 0.f;

	MovementModeType = ECharacterMovementModeType::Movement_OnGround;
	MovementStateType = ECharacterMovementStateType::Movement_Idle;
	StandType = ECharacterStandType::Character_Stand;
	StandLastFrame = ECharacterStandType::Character_Stand;

	MovementModeLastFrame = MovementModeType;

	bHasVelocity = false;
	
}

void UAbyssAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AAbyssHeroBase* InCharacter = Cast<AAbyssHeroBase>(TryGetPawnOwner()))
	{
		if (UCharacterMovementComponent* InCharacterMovementComponent = InCharacter->GetCharacterMovement())
		{
			Velocity = InCharacterMovementComponent->Velocity;
			SpeedDirection = InCharacterMovementComponent->Velocity;
			VelocityLastFrame = InCharacterMovementComponent->Velocity;
		}
	}
}

void UAbyssAnimInstanceBase::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	
	UpdateSpeed(DeltaSeconds);

	if (AAbyssHeroBase* InCharacter = Cast<AAbyssHeroBase>(TryGetPawnOwner()))
	{
		bCrouch = InCharacter->IsCrouch();
		bDie = InCharacter->IsDie();

		CharacterTransform = InCharacter->GetActorTransform();
		if (AController* InController = InCharacter->GetController())
		{
			ControllerRotator = InController->GetControlRotation();
		}
	}
}


void UAbyssAnimInstanceBase::UpdateSpeed(float DeltaSeconds)
{
    if (GetWorld())
    {
        if (ACharacter* InCharacter = Cast<ACharacter>(GetOwningActor()))
        {
            if (UCharacterMovementComponent* InCharacterMovementComponent = InCharacter->GetCharacterMovement())
            {
                Velocity = InCharacterMovementComponent->Velocity;
                SpeedDirection = InCharacterMovementComponent->Velocity;
                Speed = SpeedDirection.Size2D();
                Acceleration = InCharacterMovementComponent->GetCurrentAcceleration();

                bHasVelocity = Speed > 5.f;
                if (bHasVelocity)
                {
                    LastNonZeroVelocity = Velocity;
                }

                if (Speed != 0.f)
                {
                    SpeedDirection = InCharacter->GetRootComponent()->GetComponentTransform().
                        InverseTransformVector(SpeedDirection);

                    SpeedDirection.Normalize();
                }

                bAcceleration = InCharacterMovementComponent->GetCurrentAcceleration().Size() != 0.f;
                bInAir = InCharacterMovementComponent->IsFalling();

                VelocityAcceleration = (SpeedDirection - VelocityLastFrame) / FMath::Max(GetWorld()->DeltaTimeSeconds, 0.1f);
                MovementModeLastFrame = MovementModeType;

                switch (InCharacterMovementComponent->MovementMode)
                {
                case MOVE_None:
                case MOVE_Walking:
                case MOVE_NavWalking:
                {
                    MovementModeType = ECharacterMovementModeType::Movement_OnGround;
                    break;
                }
                case MOVE_Falling:
                    MovementModeType = ECharacterMovementModeType::Movement_InAir;
                    break;
                case MOVE_Swimming:
                    break;
                case MOVE_Flying:
                    break;
                case MOVE_Custom:
                    break;
                case MOVE_MAX:
                    break;
                }

                MovementStateLastFrame = MovementStateType;

                if (IsMoving())
                {
                    MovementStateType = ECharacterMovementStateType::Movement_Moving;
                }
                else
                {
                    MovementStateType = ECharacterMovementStateType::Movement_Idle;
                }

                StandLastFrame = StandType;

                if (bCrouch)
                {
                    StandType = ECharacterStandType::Character_Crouch;
                }
                else
                {
                    StandType = ECharacterStandType::Character_Stand;
                }

                VelocityLastFrame = InCharacterMovementComponent->Velocity;
            }
        }
    }
}

FVector UAbyssAnimInstanceBase::CalculateRelativeAccelerationAmount() const
{
    if (GetOwningActor())
    {
        if (UCharacterMovementComponent* InCharacterMovementComponent = Cast<UCharacterMovementComponent>(GetOwningActor()->FindComponentByClass(UCharacterMovementComponent::StaticClass())))
        {
            if (InCharacterMovementComponent->MaxAcceleration > 0.f && InCharacterMovementComponent->GetMaxBrakingDeceleration() > 0.f)
            {
                if (FVector::DotProduct(Acceleration, Velocity) > 0.f)
                {
                    FVector ClampVelocity = VelocityAcceleration.GetClampedToMaxSize(InCharacterMovementComponent->MaxAcceleration);
                    FVector NewVector = ClampVelocity / InCharacterMovementComponent->MaxAcceleration;

                    return CharacterTransform.GetRotation().UnrotateVector(NewVector);
                }
                else
                {
                    FVector ClampVelocity = VelocityAcceleration.GetClampedToMaxSize(InCharacterMovementComponent->GetMaxBrakingDeceleration());
                    FVector NewVector = ClampVelocity / InCharacterMovementComponent->MaxAcceleration;

                    return CharacterTransform.GetRotation().UnrotateVector(NewVector);
                }
            }
        }
    }

    return FVector::ZeroVector;
}

FVector UAbyssAnimInstanceBase::GetActorForwardVector() const
{
    if (AActor* InOwnerActor = GetOwningActor())
    {
        return InOwnerActor->GetActorForwardVector();
    }

    return FVector::ZeroVector;
}

FRotator UAbyssAnimInstanceBase::GetActorRotation() const
{
    if (AActor* InOwnerActor = GetOwningActor())
    {
        return InOwnerActor->GetActorRotation();
    }

    return FRotator::ZeroRotator;
}

FTransform UAbyssAnimInstanceBase::GetActorTransform() const
{
    if (AActor* InOwnerActor = GetOwningActor())
    {
        return InOwnerActor->GetActorTransform();
    }

    return FTransform::Identity;
}

bool UAbyssAnimInstanceBase::IsMoving() const
{
    return Speed > 4.f && bAcceleration;
}
