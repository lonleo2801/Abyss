// Copyright (c) 2025 Leon Lee


#include "AbyssPickupActor.h"

#include "Abyss.h"
#include "Components/SphereComponent.h"



// Sets default values
AAbyssPickupActor::AAbyssPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	//SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
	
	// 1. 初始化 Mesh
	PickupMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PickupMesh"));
	SetRootComponent(PickupMesh);
	
	// 默认开启物理（假设生成出来是在地上的）
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PickupMesh->SetCollisionObjectType(ECC_PhysicsBody);
	// 默认全部阻挡
	PickupMesh->SetCollisionResponseToAllChannels(ECR_Block);
	// 关键通道精细控制（按需改）
	PickupMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);    // 可被射线检测
	PickupMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);        // 不挡相机
	PickupMesh->SetCollisionResponseToChannel(ECC_Interaction, ECR_Ignore);   // 交互用 Sphere 负责
	// 开启物理
	PickupMesh->SetSimulatePhysics(true);
	PickupMesh->SetEnableGravity(true);

    
	// 2. 初始化交互球
	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetupAttachment(PickupMesh);
	PickupSphere->SetSphereRadius(80.f);
    
	// 交互球只负责 Query (射线检测)，不负责物理碰撞
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    
	// Interaction Channel 
	PickupSphere->SetCollisionResponseToChannel(ECC_Interaction, ECR_Block); 
}

FInteractionData AAbyssPickupActor::GetInteractionData_Implementation()
{
	FInteractionData Data;
	Data.ActionText = PickupText;
	Data.InteractionDuration = InteractionDuration;
	return Data;
}

void AAbyssPickupActor::OpenPickupMeshSimulatePhysics(bool bOpenPhysics)
{
	if (!PickupMesh)
	{
		return;
	}

	if (bOpenPhysics)
	{
		// === 掉在地上：开启物理 + 完整碰撞 ===
		PickupMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		PickupMesh->SetCollisionObjectType(ECC_PhysicsBody);
		PickupMesh->SetCollisionResponseToAllChannels(ECR_Block);

		// 按需微调通道
		PickupMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);    // 可被射线检测
		PickupMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);        // 不挡相机
		PickupMesh->SetCollisionResponseToChannel(ECC_Interaction, ECR_Ignore);   // 交互用 Sphere 负责

		PickupMesh->SetSimulatePhysics(true);
		PickupMesh->SetEnableGravity(true);

		// 交互球开启
		if (PickupSphere)
		{
			PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}
	else
	{
		// === 拿在手上：关闭物理 + 关闭碰撞 ===
		PickupMesh->SetSimulatePhysics(false);
		PickupMesh->SetEnableGravity(false);

		PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// 交互球关闭
		if (PickupSphere)
		{
			PickupSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}







