// Copyright (c) 2025 Leon Lee


#include "WeaponActor.h"

#include "WeaponInstance.h"
#include "Characters/AbyssCharacterBase.h"
#include "Characters/Hero/Character/AbyssHeroBase.h"
#include "Components/BoxComponent.h"
#include "Components/Combat/AbyssCombatComponentBase.h"
#include "Components/Combat/AbyssCombatComponent_Hero.h"
#include "Data/AbyssDataRegistryManager.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AWeaponActor::AWeaponActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Mesh->SetGenerateOverlapEvents(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapPickup);

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PickupCollision"));
	CombatCollision->SetupAttachment(Mesh);
	CombatCollision->InitBoxExtent(FVector(50.f, 50.f, 50.f));
	CombatCollision->SetCollisionObjectType(ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	
}

void AWeaponActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponActor, bPickedUp);
	DOREPLIFETIME(AWeaponActor, AttachedCharacter);
}

void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	// 异步加载网格
	if (const FAbyssWeaponDataRow* DataRow = UAbyssDataRegistryManager::FindCachedSync<FAbyssWeaponDataRow>(WeaponId.ItemName, WeaponId.RegistryType))
	{
		if (USkeletalMesh* LoadedMesh = DataRow->Mesh.LoadSynchronous())
		{
			Mesh->SetSkeletalMesh(LoadedMesh);
		}
	}
}


void AWeaponActor::OnOverlapPickup(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp,
                                   int32 BodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (HasAuthority())
	{
		if (AAbyssHeroBase* Char = Cast<AAbyssHeroBase>(Other))
		{
			ServerHandlePickup(Char);
		}
	}
}

void AWeaponActor::ServerHandlePickup_Implementation(AAbyssHeroBase* PickingHero)
{
	if (bPickedUp) return;
	if (!PickingHero) return;
	
	if (IAbyssCombatInterface* CombatInterface = Cast<IAbyssCombatInterface>(PickingHero))
	{
		Cast<UAbyssCombatComponent_Hero>(CombatInterface->GetCombatComponent())->EquipWeapon(WeaponId);
		bPickedUp = true;
		AttachedCharacter = PickingHero;
		AttachToCharacter(AttachedCharacter);
	}
}

void AWeaponActor::OnRep_AttachedCharacter()
{
	if (bPickedUp && AttachedCharacter)
	{
		AttachToCharacter(AttachedCharacter);
	}
	else
	{
		DetachFromCharacter();
	}
}


void AWeaponActor::AttachToCharacter(AAbyssHeroBase* PickingHero)
{
	// 附加到角色右手插槽
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(PickingHero->GetAnimMesh(), AttachRules, FName("weapon_r"));

	// 关闭拾取碰撞
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//打开攻击碰撞
	CombatCollision->SetGenerateOverlapEvents(true);

	// 可选：播放拾取动画或特效
	UE_LOG(LogTemp, Warning, TEXT("%s attached to %s"), *GetName(), *PickingHero->GetName());
}


void AWeaponActor::DetachFromCharacter()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// 打开拾取碰撞
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// 关闭攻击碰撞
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	bPickedUp = false;
	AttachedCharacter = nullptr;
}




