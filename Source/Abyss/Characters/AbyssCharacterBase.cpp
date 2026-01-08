// Copyright (c) 2025 Leon Lee


#include "AbyssCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "Abyss.h"
#include "MotionWarpingComponent.h"
#include "AbilitySystem/AbyssAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Debuff/DebuffNiagaraComponent.h"
#include "Data/AbyssDataRegistryManager.h"
#include "Data/RegistryTypeDefine.h"
#include "Data/DataTables/AbyssAbilityInfoRow.h"
#include "Data/DataTables/AbyssHeroClassInfoRow.h"
#include "GameplayTags/AbyssTags.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


namespace AbyssCharacterTags
{
	const FName Hero = "Hero";
	const FName AnimRedirect = "AnimRedirect";
	const FName Enemy = "Enemy";
}

// Sets default values
AAbyssCharacterBase::AAbyssCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
	
	BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnDebuffComponent");
	BurnDebuffComponent->SetupAttachment(GetRootComponent());
	BurnDebuffComponent->DebuffTag = AbyssTags::SetByCaller::Debuff::Fire;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_CrosshairTrace, ECR_Block);
	
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(false);
	
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}


UAbyssCombatComponentBase* AAbyssCharacterBase::GetCombatComponent() const
{
	return nullptr;
}

void AAbyssCharacterBase::Die(const FVector& DeathImpulse)
{
	MulticastHandleDeath(DeathImpulse);
}

UAbilitySystemComponent* AAbyssCharacterBase::GetAbilitySystemComponent() const
{
	return nullptr;
}

UAttributeSet* AAbyssCharacterBase::GetAttributeSet() const
{
	return nullptr;
}

FOnASCRegistered AAbyssCharacterBase::GetOnAscRegisteredDelegate() const
{
	return OnAscRegistered;
}

FOnDeathSignature AAbyssCharacterBase::GetOnDeathDelegate() const
{
	return OnDeathDelegate;
}

void AAbyssCharacterBase::InitializeDefaultAttributes() const
{
	
}

void AAbyssCharacterBase::RotateToTarget(AActor* RotateTarget)
{
	if (!IsValid(RotateTarget)) return;

	const FVector MyLocation = GetActorLocation();
	const FVector TargetLocation = RotateTarget->GetActorLocation();

	// 计算目标方向
	const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(MyLocation, TargetLocation);

	// 当前朝向
	const FRotator CurrentRotation = GetActorRotation();

	// 平滑插值旋转，RotationSpeed 可调，比如 5.f~10.f
	constexpr float RotationSpeed = 8.0f;
	const FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);

	SetActorRotation(SmoothedRotation);
	
}


void AAbyssCharacterBase::InitAbilityActorInfo()
{
}

void AAbyssCharacterBase::AddCharacterStartupAbilities() const
{
	
}

void AAbyssCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{

}

void AAbyssCharacterBase::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance,this);
		GetMesh()->SetMaterial(0,DynamicMaterialInstance);
		StartDissolveTimeline(DynamicMaterialInstance);
	}
	//通知武器actor溶解
}

void AAbyssCharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	UGameplayStatics::PlaySoundAtLocation(this,DeathSound,GetActorLocation(),GetActorRotation());
	
	/*Detach Weapon 在CombatComponet
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->AddImpulse(DeathImpulse*0.1f,NAME_None,true);*/

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse,NAME_None,true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dissolve();
	bDead = true;
	BurnDebuffComponent->Deactivate();
	//StunDebuffComponent->Deactivate();
	OnDeathDelegate.Broadcast(this);
}








