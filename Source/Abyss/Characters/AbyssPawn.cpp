// Copyright (c) 2025 Leon Lee


#include "AbyssPawn.h"

#include "AbilitySystem/AbyssAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/AbyssAttributeSet.h"
#include "Components/Combat/AbyssCombatComponent_Hero.h"
#include "Data/AbyssDataRegistryManager.h"
#include "Data/RegistryTypeDefine.h"
#include "Data/DataTables/AbyssHeroClassInfoRow.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AbyssPlayerState.h"


// Sets default values
AAbyssPawn::AAbyssPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAbyssPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAbyssPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAbyssPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	// Init ability actor info for the Server
	InitAbilityActorInfo();
}

void AAbyssPawn::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	// Init ability actor info for the Client
	InitAbilityActorInfo();
}


UAbyssCombatComponentBase* AAbyssPawn::GetCombatComponent() const
{
	return HeroCombatComponent;
}

void AAbyssPawn::Die(const FVector& DeathImpulse)
{
	MulticastHandleDeath(DeathImpulse);
}

FOnASCRegistered AAbyssPawn::GetOnAscRegisteredDelegate() const
{
	return OnAscRegistered;
}

FOnDeathSignature AAbyssPawn::GetOnDeathDelegate() const
{
	return OnDeathDelegate;
}

UAbilitySystemComponent* AAbyssPawn::GetAbilitySystemComponent() const
{
	const AAbyssPlayerState* PS = GetPlayerState<AAbyssPlayerState>();
	return PS ? PS->GetAbilitySystemComponent() : nullptr;
}

UAttributeSet* AAbyssPawn::GetAttributeSet() const
{
	const AAbyssPlayerState* PS = GetPlayerState<AAbyssPlayerState>();
	return PS ? PS->GetAttributeSet() : nullptr;
}

void AAbyssPawn::InitializeDefaultAttributes() const
{
	const FAbyssHeroClassInfoRow* CharacterClassInfo =
	UAbyssDataRegistryManager::FindCachedSyncByTag<FAbyssHeroClassInfoRow>(CharacterTypeTag, AbyssRegistryType::HeroClassInfo);

	if (CharacterClassInfo)
	{
		ApplyEffectToSelf(CharacterClassInfo->InitAttributesEffect, 1.0f);
	}
}

void AAbyssPawn::InitAbilityActorInfo()
{
	AAbyssPlayerState* AbyssPlayerState = Cast<AAbyssPlayerState>(GetPlayerState());
	check(AbyssPlayerState);
	AbyssPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AbyssPlayerState, this);

	Cast<UAbyssAbilitySystemComponent>(AbyssPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	OnAscRegistered.Broadcast(GetAbilitySystemComponent());
	
	if (HasAuthority())
	{
		//初始化属性
		InitializeDefaultAttributes();
		AbyssPlayerState->InitRuntimeData(this,CharacterTypeTag);
	}
}

void AAbyssPawn::AddCharacterStartupAbilities() const
{
	if (!HasAuthority()) return;
	
	const FAbyssHeroClassInfoRow* CharacterClassInfo =
		UAbyssDataRegistryManager::FindCachedSyncByTag<FAbyssHeroClassInfoRow>(CharacterTypeTag, AbyssRegistryType::HeroClassInfo);
	
	if (CharacterClassInfo)
	{
		TArray<FGameplayTag> StartupAbilityTags(CharacterClassInfo->StartupAbilitiesTags);
		UAbyssAbilitySystemComponent *AbyssASC = Cast<UAbyssAbilitySystemComponent>(GetAbilitySystemComponent());
		TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
		for (const FGameplayTag& AbilityTag : StartupAbilityTags)
		{
			const FAbyssAbilityInfoRow* AbilityInfo =
				UAbyssDataRegistryManager::FindCachedSyncByTag<FAbyssAbilityInfoRow>(AbilityTag, AbyssRegistryType::AbilityInfo);
			
			if (AbilityInfo)
			{
				StartupAbilities.Add(AbilityInfo->Ability);
			}
		}
		AbyssASC->AddCharacterStartupAbilities(StartupAbilities);
	}
}

void AAbyssPawn::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}


void AAbyssPawn::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	UGameplayStatics::PlaySoundAtLocation(this,DeathSound,GetActorLocation(),GetActorRotation());
	
	bDead = true;

	OnDeathDelegate.Broadcast(this);
	
	BP_HandleDeath(DeathImpulse);
}



