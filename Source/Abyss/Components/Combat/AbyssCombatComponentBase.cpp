// Copyright (c) 2025 Leon Lee


#include "AbyssCombatComponentBase.h"

#include "AbyssDebugHelper.h"
#include "MotionWarpingComponent.h"
#include "Characters/AbyssCharacterBase.h"
#include "Player/AbyssPlayerController.h"


void UAbyssCombatComponentBase::SetFacingTarget(const FVector& FacingVector)
{
	const AAbyssCharacterBase* AbyssCharacter = Cast<AAbyssCharacterBase>(GetOwner());
	
	AbyssCharacter->MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation("FacingTarget", FacingVector);
}

void UAbyssCombatComponentBase::SetFacingToCameraDirection()
{
	const AAbyssCharacterBase* AbyssCharacter = Cast<AAbyssCharacterBase>(GetOwner());
	const AAbyssPlayerController* PlayerController = GetOwningController<AAbyssPlayerController>();

	const FRotator ControlRot = PlayerController->PlayerCameraManager->GetCameraRotation();
	const FVector Dir = ControlRot.Vector();

	const FVector FacingTarget = AbyssCharacter->GetActorLocation() + Dir.GetSafeNormal2D()*100;
	

	AbyssCharacter->MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation("FacingTarget", FacingTarget);
	
}
