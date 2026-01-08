// Copyright (c) 2025 Leon Lee


#include "AbyssEnemyBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AbyssAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/AbyssAttributeSet.h"
#include "Components/Combat/AbyssCombatComponent_Enemy.h"
#include "Data/AbyssDataRegistryManager.h"
#include "Data/RegistryTypeDefine.h"
#include "Data/DataTables/AbyssAbilityInfoRow.h"
#include "Data/DataTables/AbyssEnemyClassInfoRow.h"
#include "Data/DataTables/AbyssHeroClassInfoRow.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/EnemyWidgets/EnemyHealthWidgetComponent.h"
#include "UI/ViewMode/AbyssViewModel_EnemyAttributes.h"


// Sets default values
AAbyssEnemyBase::AAbyssEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbyssAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	AttributeSet = CreateDefaultSubobject<UAbyssAttributeSet>("AttributeSet");
	EnemyCombatComponent = CreateDefaultSubobject<UAbyssCombatComponent_Enemy>("EnemyCombatComponent");
	
	HealthWidgetComponent = CreateDefaultSubobject<UEnemyHealthWidgetComponent>("WidgetComponent");
	HealthWidgetComponent->SetupAttachment(GetRootComponent());
	
	Tags.Add(AbyssCharacterTags::Enemy);
}

UAbyssCombatComponentBase* AAbyssEnemyBase::GetCombatComponent() const
{
	return EnemyCombatComponent;
}

UAbilitySystemComponent* AAbyssEnemyBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AAbyssEnemyBase::GetAttributeSet() const
{
	return AttributeSet;
}

void AAbyssEnemyBase::AddCharacterStartupAbilities() const
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

void AAbyssEnemyBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AAbyssEnemyBase::InitializeDefaultAttributes() const
{
	const FAbyssEnemyClassInfoRow* CharacterClassInfo =
	UAbyssDataRegistryManager::FindCachedSyncByTag<FAbyssEnemyClassInfoRow>(CharacterTypeTag, AbyssRegistryType::EnemyClassInfo);

	if (CharacterClassInfo)
	{
		ApplyEffectToSelf(CharacterClassInfo->InitAttributesEffect, 1.0f);
	}
}

void AAbyssEnemyBase::Die(const FVector& DeathImpulse)
{
	SetLifeSpan(LifeSpan);
	//TODO: 设置行为树相关内容
	/*if (AuraAIController)
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
	}*/
	HealthWidgetComponent->DestroyComponent();
	Super::Die(DeathImpulse);
}

void AAbyssEnemyBase::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(250);
	//Weapon->SetRenderCustomDepth(true);
	//Weapon->SetCustomDepthStencilValue(250);
}

void AAbyssEnemyBase::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	//Weapon->SetRenderCustomDepth(false);
}

// Called when the game starts or when spawned
void AAbyssEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();
	
	if (HasAuthority())
	{
		//初始化初始能力
		AddCharacterStartupAbilities();
	}
}

void AAbyssEnemyBase::InitAbilityActorInfo()
{
	
	GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);

	Cast<UAbyssAbilitySystemComponent>(GetAbilitySystemComponent())->AbilityActorInfoSet();
	
	if (HasAuthority())
	{
		//初始化ViewModel
		InitAttributesViewModel();
	
		//初始化初始属性
		InitializeDefaultAttributes();
	}
	
	OnAscRegistered.Broadcast(GetAbilitySystemComponent());
	
}

void AAbyssEnemyBase::InitAttributesViewModel()
{
	EnemyAttributesViewModel = NewObject<UAbyssViewModel_EnemyAttributes>(this);
	if (IsValid(EnemyAttributesViewModel))
	{
		UAbyssAttributeSet* AbyssAttributeSet = Cast<UAbyssAttributeSet>(GetAttributeSet());
		GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(AbyssAttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				EnemyAttributesViewModel->SetHealth(Data.NewValue);
			}	
		);
		GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(AbyssAttributeSet->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				EnemyAttributesViewModel->SetMaxHealth(Data.NewValue);
			}	
		);
	}
	HealthWidgetComponent->InitialHealthWidget(this);
}















