// Copyright (c) 2025 Leon Lee


#include "AbyssBlueprintLibrary.h"

#include "Characters/AbyssCharacterBase.h"
#include "Kismet/GameplayStatics.h"

EHitDirection UAbyssBlueprintLibrary::GetHitDirection(const FVector& TargetForward, const FVector& ToInstigator)
{
	const float Dot = FVector::DotProduct(TargetForward, ToInstigator);
	if (Dot < -0.5f)
	{
		return EHitDirection::Back;
	}
	if (Dot < 0.5f)
	{
		// Either Left or Right
		const FVector Cross = FVector::CrossProduct(TargetForward, ToInstigator);
		if (Cross.Z < 0.f)
		{
			return EHitDirection::Left;
		}
		return EHitDirection::Right;
	}
	return EHitDirection::Forward;
}

FName UAbyssBlueprintLibrary::GetHitDirectionName(const EHitDirection& HitDirection)
{
	switch (HitDirection)
	{
		case EHitDirection::Left: return FName("Left");
		case EHitDirection::Right: return FName("Right");
		case EHitDirection::Forward: return FName("Forward");
		case EHitDirection::Back: return FName("Back");
		default: return FName("None");
	}
}

UAbyssCombatComponentBase* UAbyssBlueprintLibrary::NativeGetCombatComponentFromActor(AActor* InActor)
{
	check(InActor);

	if (IAbyssCombatInterface* CombatInterface = Cast<IAbyssCombatInterface>(InActor))
	{
		return CombatInterface->GetCombatComponent();
	}
	return nullptr;
}

UAbyssCombatComponentBase* UAbyssBlueprintLibrary::BP_GetCombatComponentFromActor(AActor* InActor,EAbyssValidType& OutValidType)
{
	UAbyssCombatComponentBase* CombatComponent = NativeGetCombatComponentFromActor(InActor);

	OutValidType=CombatComponent ? EAbyssValidType::Valid : EAbyssValidType::Invalid;

	return CombatComponent;
}


