// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"

#include "CommonMessagingSubsystem.generated.h"

#define UE_API COMMONGAME_API

class FSubsystemCollectionBase;
class UCommonGameDialogDescriptor;
class UObject;

/** Possible results from a dialog */
/** 对话可能出现的结果 */
UENUM(BlueprintType)
enum class ECommonMessagingResult : uint8
{
	/** The "yes" button was pressed */
	/** “是”按钮已被按下 */
	Confirmed,
	/** The "no" button was pressed */
	/** “取消”按钮已被按下 */
	Declined,
	/** The "ignore/cancel" button was pressed */
	/** “忽略/取消”按钮已被按下 */
	Cancelled,
	/** The dialog was explicitly killed (no user input) */
	/** 该对话框已被明确关闭（未收到用户输入） */
	Killed,
	Unknown UMETA(Hidden)
};

DECLARE_DELEGATE_OneParam(FCommonMessagingResultDelegate, ECommonMessagingResult /* Result */);

/**
 * 
 */
UCLASS(MinimalAPI, config = Game)
class UCommonMessagingSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UCommonMessagingSubsystem() { }

	UE_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	UE_API virtual void Deinitialize() override;
	UE_API virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	UE_API virtual void ShowConfirmation(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback = FCommonMessagingResultDelegate());
	UE_API virtual void ShowError(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback = FCommonMessagingResultDelegate());

private:

};

#undef UE_API
