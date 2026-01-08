// Copyright (c) 2025 Leon Lee


#include "AbyssFireBolt.h"

#include "Abyss.h"
#include "AbilitySystem/BlueprintLibrary/AbyssAbilitySystemLibrary.h"
#include "Actors/Projectiles/AbyssProjectile.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interfaces/AbyssCombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"


FString UAbyssFireBolt::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>FIRE BOLT</>\n\n"

			// Level
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			
			"<Default>Launches a bolt of fire, "
			"exploding on impact and dealing: </>"

			// Damage
			"<Damage>%d</><Default> fire damage with"
			" a chance to burn</>"),

			// Values
			Level,
			ManaCost,
			Cooldown,
			ScaledDamage);
	}
	else
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>FIRE BOLT</>\n\n"

			// Level
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Number of FireBolts
			"<Default>Launches %d bolts of fire, "
			"exploding on impact and dealing: </>"

			// Damage
			"<Damage>%d</><Default> fire damage with"
			" a chance to burn</>"),

			// Values
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level, NumProjectiles),
			ScaledDamage);		
	}
}

FString UAbyssFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
			// Title
			"<Title>NEXT LEVEL: </>\n\n"

			// Level
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Number of FireBolts
			"<Default>Launches %d bolts of fire, "
			"exploding on impact and dealing: </>"

			// Damage
			"<Damage>%d</><Default> fire damage with"
			" a chance to burn</>"),

			// Values
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level, NumProjectiles),
			ScaledDamage);
}

void UAbyssFireBolt::SpawnProjectiles(FVector ProjectileTargetLocation, FGameplayTag SocketTag, bool bOverridePitch,float PitchOverride, AActor* HomingTarget)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	IAbyssCombatInterface* CombatInterface = Cast<IAbyssCombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface ==  nullptr) return;
	//const FVector SocketLocation = CombatInterface->GetCombatSocketLocation(SocketTag);
	const FVector SocketLocation = GetAvatarActorFromActorInfo()->GetActorLocation();

	FRotator Rotation;
	if (ProjectileTargetLocation.IsZero())
	{
		Rotation = GetControlRotationSafe();
	}
	else
	{
		Rotation  = (ProjectileTargetLocation - SocketLocation).Rotation();
	}
	
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}
	else
	{
		Rotation.Pitch = 0;
	}
	
	const FVector Forward = Rotation.Vector();
	//const int32 EffectiveNumProjectiles = FMath::Min(GetAbilityLevel(), MaxNumProjectiles);
	
	TArray<FRotator> Rotators = UAbyssAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, ProjectileSpread, MaxNumProjectiles);


	for (const FRotator& Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotator.Quaternion());
		
		AAbyssProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAbyssProjectile>(
			ProjectileClass,             
			SpawnTransform,              
			GetOwningActorFromActorInfo(),     
			Cast<APawn>(GetOwningActorFromActorInfo()), 
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);  
		
		if (HomingTarget && HomingTarget->Implements<UAbyssCombatInterface>())
		{
			Projectile->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
			Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMin, HomingAccelerationMax);
			Projectile->ProjectileMovement->bIsHomingProjectile = bLaunchHomingProjectiles;
		}
		else
		{
			
		}
		
		//还不知道命中了谁，所以TargetActor参数传入nullptr
		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		
		Projectile->FinishSpawning(SpawnTransform);
	}
}


















