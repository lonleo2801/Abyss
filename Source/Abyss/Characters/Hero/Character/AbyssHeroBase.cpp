// Copyright (c) 2025 Leon Lee


#include "AbyssHeroBase.h"

#include "AbilitySystemComponent.h"
#include "AbyssDebugHelper.h"
#include "AbilitySystem/AbyssAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/AbyssAttributeSet.h"
#include "Camera/CameraComponent.h"
#include "Player/AbyssPlayerState.h"
#include "Components/CapsuleComponent.h"
#include "Components/Combat/AbyssCombatComponent_Hero.h"
#include "Data/AbyssDataRegistryManager.h"
#include "Data/RegistryTypeDefine.h"
#include "Data/DataTables/AbyssAttributeInfoRow.h"
#include "Data/DataTables/AbyssHeroClassInfoRow.h"
#include "Data/DataTables/AbyssInputConfigRow.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameplayTags/AbyssTags.h"
#include "Weapons/AbyssWeaponFunctionLibrary.h"


// Sets default values
AAbyssHeroBase::AAbyssHeroBase()
{

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->SocketOffset = FVector(0.0f, 55.0f, 65.0f);
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	HeroCombatComponent = CreateDefaultSubobject<UAbyssCombatComponent_Hero>(TEXT("HeroCombatComponent"));
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	
	AnimMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("AnimMesh"));
	AnimMesh->SetupAttachment(GetMesh());
	AnimMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	GetMesh()->SetVisibility(false);
	GetMesh()->SetHiddenInGame(false);
	
	AnimMesh->ComponentTags.AddUnique(AbyssCharacterTags::AnimRedirect);
	Tags.Add(AbyssCharacterTags::Hero);
}

// Called to bind functionality to input
void AAbyssHeroBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UAbilitySystemComponent* AAbyssHeroBase::GetAbilitySystemComponent() const
{
	const AAbyssPlayerState* PS = GetPlayerState<AAbyssPlayerState>();
	return PS ? PS->GetAbilitySystemComponent() : nullptr;
}

UAttributeSet* AAbyssHeroBase::GetAttributeSet() const
{
	const AAbyssPlayerState* PS = GetPlayerState<AAbyssPlayerState>();
	return PS ? PS->GetAttributeSet() : nullptr;
}

void AAbyssHeroBase::AddCharacterStartupAbilities() const
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

void AAbyssHeroBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AAbyssHeroBase::InitializeDefaultAttributes() const
{
	const FAbyssHeroClassInfoRow* CharacterClassInfo =
	UAbyssDataRegistryManager::FindCachedSyncByTag<FAbyssHeroClassInfoRow>(CharacterTypeTag, AbyssRegistryType::HeroClassInfo);

	if (CharacterClassInfo)
	{
		ApplyEffectToSelf(CharacterClassInfo->InitAttributesEffect, 1.0f);
	}
}

UAbyssCombatComponentBase* AAbyssHeroBase::GetCombatComponent() const
{
	return HeroCombatComponent;
}

void AAbyssHeroBase::LevelUp(int32 NewLevel)
{
	//TODO: 升级特效
	AbyssDebug::Print(FString::Printf(TEXT("Level Up: %d"), NewLevel));
}


void AAbyssHeroBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init ability actor info for the Server
	InitAbilityActorInfo();
	
	//AddCharacterStartupAbilities();
}

void AAbyssHeroBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init ability actor info for the Client
	InitAbilityActorInfo();
}

// Called when the game starts or when spawned
void AAbyssHeroBase::BeginPlay()
{
	Super::BeginPlay();

	FAbyssWeaponDataRow SwordData;
	if (UAbyssWeaponFunctionLibrary::GetWeaponDataSync(FName("Sword"), SwordData))
	{
		//AbyssDebug::Print(FString::Printf(TEXT("Weapon Name: %s, Damage: %f"), *SwordData.DisplayName.ToString(), SwordData.Damage));
	}
	
	UAbyssDataRegistryManager::FindCachedAsync(FName("AbyssTags.InputTag.RMB"),AbyssRegistryType::InputConfig,
		FDataRegistryItemAcquiredCallback::CreateLambda([](const FDataRegistryAcquireResult& Result)
		{
			const FAbyssInputConfigRow* inputData = Result.GetItem<FAbyssInputConfigRow>();
			//AbyssDebug::Print(FString::Printf(TEXT("Weapon Name: %s"), *inputData->InputTag.ToString()));
		}));
	
	const FAbyssAttributeInfoRow* AttributeData = UAbyssDataRegistryManager::FindCachedSyncByTag<FAbyssAttributeInfoRow>(AbyssTags::Attributes::CooldownReduction, AbyssRegistryType::Attributes);
	//AbyssDebug::Print(FString::Printf(TEXT("Attribute Name: %s"), *AttributeData->AttributeName.ToString()));


	TFunctionRef<void(const FName& Name, const FAbyssInputConfigRow& Item)> Predicate = [](const FName& Name, const FAbyssInputConfigRow& Item)
	{
		//AbyssDebug::Print(FString::Printf(TEXT("Input Name: %s，Input Tag: %s"), *Name.ToString(), *Item.InputTag.ToString()));
	};
	UAbyssDataRegistryManager::ForEachCached(AbyssRegistryType::InputConfig,Predicate);
	
}



bool AAbyssHeroBase::IsAttack()
{
	return false;
}

bool AAbyssHeroBase::IsCrouch()
{
	return false;
}

bool AAbyssHeroBase::IsDie()
{
	return false;
}

ECharacterGaitType AAbyssHeroBase::GetGaitType()
{
	return ECharacterGaitType::Gait_Walk;
}

bool AAbyssHeroBase::IsJustLanded()
{
	return false;
}

FVector AAbyssHeroBase::GetLandVelocity()
{
	return FVector::ZeroVector;
}

void AAbyssHeroBase::InitAbilityActorInfo()
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



