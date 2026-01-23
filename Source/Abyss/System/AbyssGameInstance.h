// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "AbyssGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssGameInstance :  public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()

public:
	UAbyssGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Init() override;
	virtual void Shutdown() override;
	
	virtual int32 AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId) override;
	virtual bool RemoveLocalPlayer(ULocalPlayer* ExistingPlayer) override;
	
private:
	/** This is the primary player*/
	TWeakObjectPtr<ULocalPlayer> PrimaryPlayer;
};
