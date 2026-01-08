// Copyright (c) 2025 Leon Lee


#include "AbyssUIManagerSubsystem.h"

#include "CommonLocalPlayer.h"
#include "GameUIPolicy.h"
#include "PrimaryGameLayout.h"
#include "Components/SlateWrapperTypes.h"
#include "GameFramework/HUD.h"

class FSubsystemCollectionBase;

UAbyssUIManagerSubsystem::UAbyssUIManagerSubsystem()
{
}

void UAbyssUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UAbyssUIManagerSubsystem::Tick), 0.0f);

}

void UAbyssUIManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();

	
	FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
}

bool UAbyssUIManagerSubsystem::Tick(float DeltaTime)
{
	SyncRootLayoutVisibilityToShowHUD();
	
	return true;
}

void UAbyssUIManagerSubsystem::SyncRootLayoutVisibilityToShowHUD()
{

	if (const UGameUIPolicy* Policy = GetCurrentUIPolicy())
	{

		for (const ULocalPlayer* LocalPlayer : GetGameInstance()->GetLocalPlayers())
		{
			bool bShouldShowUI = true;

						
			if (const APlayerController* PC = LocalPlayer->GetPlayerController(GetWorld()))
			{
				const AHUD* HUD = PC->GetHUD();

				if (HUD && !HUD->bShowHUD)
				{
					bShouldShowUI = false;
				}
			}
			if (UPrimaryGameLayout* RootLayout = Policy->GetRootLayout(CastChecked<UCommonLocalPlayer>(LocalPlayer)))
			{
				const ESlateVisibility DesiredVisibility = bShouldShowUI ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed;

				if (DesiredVisibility != RootLayout->GetVisibility())
				{
					RootLayout->SetVisibility(DesiredVisibility);	
				}
				
			}
			
		}
	}
}

