// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "Engine/GameInstance.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerState.h"

//#include "UObjectIterator.h"

#include "AdvancedGameSession.generated.h"




/**
 游戏会话的快速包装器，用于添加部分封禁实现。仅在当前会话期间进行封禁
*/
UCLASS(config = Game, notplaceable)
class AAdvancedGameSession : public AGameSession
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(Transient)
	TMap<FUniqueNetIdRepl, FText> BanList;

	virtual bool BanPlayer(class APlayerController* BannedPlayer, const FText& BanReason)
	{

		if (APlayerState* PlayerState = (BannedPlayer != NULL) ? BannedPlayer->PlayerState : NULL)
		{
			FUniqueNetIdRepl UniqueNetID = PlayerState->GetUniqueId();
			bool bWasKicked = KickPlayer(BannedPlayer, BanReason);

			if (bWasKicked)
			{
				BanList.Add(UniqueNetID, BanReason);
			}

			return bWasKicked;
		}
		
		return false;
	}

	// 这实际上应该在询问游戏会话的游戏模式中处理，但我不想强制使用自定义游戏会话 AND 游戏模式
	// 如果在游戏模式中完成，我们可以在 ApproveLogin 中实际 spooling up 任何玩家信息之前进行检查
	virtual void PostLogin(APlayerController* NewPlayer) override
	{
		if (APlayerState* PlayerState = (NewPlayer != NULL) ? NewPlayer->PlayerState : NULL)
		{
			FUniqueNetIdRepl UniqueNetID = PlayerState->GetUniqueId();

			if (BanList.Contains(UniqueNetID))
			{
				KickPlayer(NewPlayer, BanList[UniqueNetID]);
			}
		}
	}
};

AAdvancedGameSession::AAdvancedGameSession(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}