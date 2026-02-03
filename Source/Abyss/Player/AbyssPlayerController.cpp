// Copyright Epic Games, Inc. All Rights Reserved.


#include "AbyssPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CommonLocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "PrimaryGameLayout.h"
#include "AbilitySystem/AbyssAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/AbyssAttributeSet.h"
#include "Characters/AbyssCharacterBase.h"
#include "Components/Input/AbyssInputComponent.h"
#include "GameFramework/Character.h"
#include "GameplayTags/AbyssTags.h"
#include "GameplayTags/AbyssUITags.h"
#include "UI/EnemyWidgets/AbyssDamageTextComponent.h"
#include "UI/SubSystem/AbyssUIManagerSubsystem.h"
#include "UI/ViewMode/AbyssViewModel_CharacterAttributes.h"
#include "UI/ViewMode/AbyssViewModel_GameMenuInteraction.h"

/**
 * 通用属性变更绑定宏
 * @param AttributeSetVar 属性集变量名
 * @param AttributeName   属性名	
 */
#define BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AttributeSetVar, AttributeName) \
	GetASC()->GetGameplayAttributeValueChangeDelegate(AbyssAttributeSet->TagsToAttributesMaps[AbyssTags::Attributes::AttributeName]()).AddLambda( \
		[this](const FOnAttributeChangeData& Data) \
		{ \
			CharacterAttributesViewModel->Set##AttributeName(Data.NewValue); \
		} \
	);\
	CharacterAttributesViewModel->Set##AttributeName(AbyssAttributeSet->Get##AttributeName());


void AAbyssPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AAbyssPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	// 将前端界面推送到主界面!
	UCommonUIExtensions::PushStreamedContentToLayer_ForPlayer(GetLocalPlayer(), AbyssTags_UI::Layer::HUD, HUDClass);

	// 初始化ViewModel
	InitCharacterAttributesViewModel();
	
	// 绑定属性修改
	BindAttributesChangeDelegateToViewModel();
}


void AAbyssPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	// 将前端界面推送到主界面!
	UCommonUIExtensions::PushStreamedContentToLayer_ForPlayer(GetLocalPlayer(), AbyssTags_UI::Layer::HUD, HUDClass);
	
	// 初始化ViewModel
	InitCharacterAttributesViewModel();
	
	if (GetASC())
	{
		// 绑定属性修改
		BindAttributesChangeDelegateToViewModel();
	}
	else if (GetPawn())
	{
		if (AAbyssCharacterBase* AbyssCharacter = Cast<AAbyssCharacterBase>(GetPawn()))
		{
			AbyssCharacter->GetOnAscRegisteredDelegate().AddLambda([this](UAbilitySystemComponent* InASC)
			{
				BindAttributesChangeDelegateToViewModel();
			});
		}
	}
	else
	{
		FTimerDelegate TimerDel;
		TimerDel.BindLambda([this]()
		{
			// 绑定属性修改
			BindAttributesChangeDelegateToViewModel();
			
		});

		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(
			Handle,
			TimerDel,
			1.f,
			false
		);
	}
}


void AAbyssPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!IsValid(InputSubsystem)) return;

	for (UInputMappingContext* Context : AbilityIMC)
	{
		InputSubsystem->AddMappingContext(Context, 0);
	}

	UAbyssInputComponent* AbyssEnhancedInputComponent = Cast<UAbyssInputComponent>(InputComponent);
	if (!IsValid(AbyssEnhancedInputComponent)) return;
	
	//AbyssEnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
	//ability
	AbyssEnhancedInputComponent->BindAbilityActions(this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAbyssPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(AbyssTags::Block::Input::AbilityPressed))
	{
		return;
	}
	if (GetASC()) GetASC()->AbilityInputTagPressed(InputTag);
}

void AAbyssPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(AbyssTags::Block::Input::AbilityReleased))
	{
		return;
	}
	if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
}

void AAbyssPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(AbyssTags::Block::Input::AbilityHeld))
	{
		return;
	}
	if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
}


UAbyssAbilitySystemComponent* AAbyssPlayerController::GetASC()
{
	if (!AbyssAbilitySystemComponent.IsValid()&&GetPawn())
	{
		AbyssAbilitySystemComponent = Cast<UAbyssAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	return AbyssAbilitySystemComponent.Get();
}

void AAbyssPlayerController::ShowDamageNumber_Implementation(float DamageAmount,ACharacter* TargetCharacter,bool bBlockHit,bool bCritHit)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		UAbyssDamageTextComponent* DamageTextComponent = NewObject<UAbyssDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageTextComponent->RegisterComponent();
		DamageTextComponent->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageTextComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageTextComponent->SetDamageText(DamageAmount,bBlockHit,bCritHit);
	}
}


void AAbyssPlayerController::InitCharacterAttributesViewModel()
{
	CharacterAttributesViewModel = NewObject<UAbyssViewModel_CharacterAttributes>(this);
	GameMenuInteractionViewModel = NewObject<UAbyssViewModel_GameMenuInteraction>(this); 
}

void AAbyssPlayerController::BindAttributesChangeDelegateToViewModel()
{
	const UAbyssAttributeSet* AbyssAttributeSet = GetASC()->GetSet<UAbyssAttributeSet>();
	
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, Health);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, MaxHealth);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, Mana);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, MaxMana);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, Stamina);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, MaxStamina);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, MoveSpeed);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, PhysicalPower);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, MagicPower);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, AttackSpeed);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, CooldownReduction);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, CritRate);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, CritDamage);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, PhysicalPenetration);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, MagicPenetration);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, Defense);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, BlockChance);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, PhysicalResistance);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, MagicResistance);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, SkillPoints);
	BIND_ATTRIBUTE_TO_VIEWMODEL_DELEGATE(AbyssAttributeSet, SkillSlotAmount);
}


void AAbyssPlayerController::ActivateAbility(const FGameplayTag& AbilityTag) const
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	if (!IsValid(ASC)) return;

	ASC->TryActivateAbilitiesByTag(AbilityTag.GetSingleTagContainer());
	
}
