// Copyright (c) 2025 Leon Lee


#include "AbyssAbilitySystemLibrary.h"
#include "Engine/OverlapResult.h"
#include "Interfaces/AbyssCombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AbyssPlayerState.h"

void UAbyssAbilitySystemLibrary::GetLiveCharacterWithinRadius(const UObject* WorldContextObject,
                                                              TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius,
                                                              const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	// query scene to see what we hit
	TArray<FOverlapResult> Overlaps;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity,
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
			FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			if (IAbyssCombatInterface* ICombatInterface = Cast<IAbyssCombatInterface>(Overlap.GetActor()))
			{
				if (!ICombatInterface->IsDead())
				{
					OutOverlappingActors.AddUnique(Overlap.GetActor());
				}
			}
		}
	}
}

void UAbyssAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors,TArray<AActor*>& OutClosestTargets,const FVector& Origin)
{
	if (Actors.Num()<= MaxTargets)
	{
		OutClosestTargets = Actors;
		return;
	}
	
	TArray<AActor*> ActorsToCheck = Actors;
	int32 NumTargetsFound = 0;
	while (NumTargetsFound < MaxTargets)
	{ 
		if (ActorsToCheck.Num()== 0) break;

		double ClosestDistance = TNumericLimits<double>::Max();
		AActor* ClosestActor;
		
		for (AActor* PotentialActor : ActorsToCheck)
		{
			const double Distance = (PotentialActor->GetActorLocation() - Origin).Length();
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = PotentialActor;
			}
		}
		ActorsToCheck.Remove(ClosestActor);
		OutClosestTargets.AddUnique(ClosestActor);
		++NumTargetsFound;
	}
}

bool UAbyssAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bBothAreHero = FirstActor->ActorHasTag(FName("Hero")) && SecondActor->ActorHasTag(FName("Hero"));
	const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	const bool bFriends= bBothAreHero || bBothAreEnemies;
	return !bFriends;
}

UAbyssPlayerAbilityRuntimeData* UAbyssAbilitySystemLibrary::GetPlayerAbilityRuntimeData(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAbyssPlayerState* PlayerState = PC->GetPlayerState<AAbyssPlayerState>())
		{
			return PlayerState->GetRuntimePlayerAbilityData();
		}
	}
	return nullptr;
}

TArray<FRotator> UAbyssAbilitySystemLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis,float Spread,int32 NumRotators)
{
	TArray<FRotator> Rotators;
	
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (NumRotators>1)
	{
		const float DeltaSpread = Spread / (NumRotators - 1);
		for (int32 i = 0; i < NumRotators; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, Axis);
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		Rotators.Add(Forward.Rotation());
	}
	
	return Rotators;
}

TArray<FVector> UAbyssAbilitySystemLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis,float Spread, int32 NumVectors)
{
	TArray<FVector> Vectors;
	
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (NumVectors>1)
	{
		const float DeltaSpread = Spread / (NumVectors - 1);
		for (int32 i = 0; i < NumVectors; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, Axis);
			Vectors.Add(Direction);
		}
	}
	else
	{
		Vectors.Add(Forward);
	}
	
	return Vectors;
}















