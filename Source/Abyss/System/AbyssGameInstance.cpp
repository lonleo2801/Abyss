// Copyright (c) 2025 Leon Lee


#include "AbyssGameInstance.h"

#include "CommonLocalPlayer.h"
#include "GameUIManagerSubsystem.h"

UAbyssGameInstance::UAbyssGameInstance(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

void UAbyssGameInstance::Init()
{
	Super::Init();

	// 这里可以初始化你的项目逻辑（数据管理、UI系统等）
	UE_LOG(LogTemp, Log, TEXT("UAbyssGameInstance::Init()"));
}

void UAbyssGameInstance::Shutdown()
{
	Super::Shutdown();
	UE_LOG(LogTemp, Log, TEXT("UAbyssGameInstance::Shutdown()"));
}

int32 UAbyssGameInstance::AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId)
{
	int32 ReturnVal = Super::AddLocalPlayer(NewPlayer, UserId);
	if (ReturnVal != INDEX_NONE)
	{
		if (!PrimaryPlayer.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("AddLocalPlayer: Set %s to Primary Player"), *NewPlayer->GetName());
			PrimaryPlayer = NewPlayer;
		}
		
		GetSubsystem<UGameUIManagerSubsystem>()->NotifyPlayerAdded(Cast<UCommonLocalPlayer>(NewPlayer));
	}
	
	return ReturnVal;
}

bool UAbyssGameInstance::RemoveLocalPlayer(ULocalPlayer* ExistingPlayer)
{
	if (PrimaryPlayer == ExistingPlayer)
	{
		//TODO: do we want to fall back to another player?
		PrimaryPlayer.Reset();
		UE_LOG(LogTemp, Log, TEXT("RemoveLocalPlayer: Unsetting Primary Player from %s"), *ExistingPlayer->GetName());
	}
	GetSubsystem<UGameUIManagerSubsystem>()->NotifyPlayerDestroyed(Cast<UCommonLocalPlayer>(ExistingPlayer));
	
	return Super::RemoveLocalPlayer(ExistingPlayer);
}
