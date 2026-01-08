// Copyright (c) 2025 Leon Lee


#include "AbyssProjectileAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/BlueprintLibrary/AbyssAbilitySystemLibrary.h"
#include "Actors/Projectiles/AbyssProjectile.h"
#include "GameplayTags/AbyssTags.h"
#include "Interfaces/AbyssCombatInterface.h"

void UAbyssProjectileAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                  const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	
}

void UAbyssProjectileAbility::SpawnProjectile(FVector ProjectileTargetLocation,FGameplayTag SocketTag,bool bOverridePitch,float PitchOverride)
{
	
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	IAbyssCombatInterface* CombatInterface = Cast<IAbyssCombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface)
	{
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
    
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());

		AAbyssProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAbyssProjectile>(
			ProjectileClass,             
			SpawnTransform,              
			GetOwningActorFromActorInfo(),     
			Cast<APawn>(GetOwningActorFromActorInfo()), 
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);  
		
		//还不知道命中了谁，所以TargetActor参数传入nullptr
		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		
		Projectile->FinishSpawning(SpawnTransform);
	}
}












