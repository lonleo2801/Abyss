// Copyright Epic Games, Inc. All Rights Reserved.

#include "Messaging/CommonMessagingSubsystem.h"

#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "UObject/UObjectHash.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CommonMessagingSubsystem)

class FSubsystemCollectionBase;
class UClass;

void UCommonMessagingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UCommonMessagingSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UCommonMessagingSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	UGameInstance* GameInstance = CastChecked<ULocalPlayer>(Outer)->GetGameInstance();
	if (GameInstance && !GameInstance->IsDedicatedServerInstance())
	{
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(GetClass(), ChildClasses, false);

		// Only create an instance if there is no override implementation defined elsewhere
		// 仅在未在其他地方定义覆盖实现的情况下才创建实例
		return ChildClasses.Num() == 0;
	}

	return false;
}

void UCommonMessagingSubsystem::ShowConfirmation(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback)
{
	
}

void UCommonMessagingSubsystem::ShowError(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback)
{
	
}
