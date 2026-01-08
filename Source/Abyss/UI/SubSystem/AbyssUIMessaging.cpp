// Copyright (c) 2025 Leon Lee


#include "AbyssUIMessaging.h"

#include "CommonLocalPlayer.h"
#include "PrimaryGameLayout.h"
#include "GameplayTags/AbyssTags.h"
#include "GameplayTags/AbyssUITags.h"

class FSubsystemCollectionBase;

void UAbyssUIMessaging::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	
	ConfirmationDialogClassPtr = ConfirmationDialogClass.LoadSynchronous();
	ErrorDialogClassPtr = ErrorDialogClass.LoadSynchronous();
}

void UAbyssUIMessaging::ShowConfirmation(UCommonGameDialogDescriptor* DialogDescriptor,
	FCommonMessagingResultDelegate ResultCallback)
{

	if (UCommonLocalPlayer* LocalPlayer = GetLocalPlayer<UCommonLocalPlayer>())
	{

		if (UPrimaryGameLayout* RootLayout = LocalPlayer->GetRootUILayout())
		{
			RootLayout->PushWidgetToLayerStack<UCommonGameDialog>(AbyssTags_UI::WidgetStack::Modal,
				ConfirmationDialogClassPtr, [DialogDescriptor, ResultCallback](UCommonGameDialog& Dialog) {
					
				Dialog.SetupDialog(DialogDescriptor, ResultCallback);
					
			});
		}
	}
}

void UAbyssUIMessaging::ShowError(UCommonGameDialogDescriptor* DialogDescriptor,
	FCommonMessagingResultDelegate ResultCallback)
{
	if (UCommonLocalPlayer* LocalPlayer = GetLocalPlayer<UCommonLocalPlayer>())
	{
		if (UPrimaryGameLayout* RootLayout = LocalPlayer->GetRootUILayout())
		{
			RootLayout->PushWidgetToLayerStack<UCommonGameDialog>(AbyssTags_UI::WidgetStack::Modal,
				ErrorDialogClassPtr, [DialogDescriptor, ResultCallback](UCommonGameDialog& Dialog) {
				Dialog.SetupDialog(DialogDescriptor, ResultCallback);
			});
		}
	}
	
}
