// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonActivatableWidget.h"
#include "CommonMessagingSubsystem.h"

#include "CommonGameDialog.generated.h"

#define UE_API COMMONGAME_API

USTRUCT(BlueprintType)
struct FConfirmationDialogAction
{
	GENERATED_BODY()

public:
	/** Required: The dialog option to provide. */
	/** 必填项：需提供的对话选项。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECommonMessagingResult Result = ECommonMessagingResult::Unknown;

	/** Optional: Display Text to use instead of the action name associated with the result. */
	/** （可选）：显示用于替代与结果相关联的操作名称的文本内容。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText OptionalDisplayText;

	bool operator==(const FConfirmationDialogAction& Other) const
	{
		return Result == Other.Result &&
			OptionalDisplayText.EqualTo(Other.OptionalDisplayText);
	}
};

UCLASS(MinimalAPI)
class UCommonGameDialogDescriptor : public UObject
{
	GENERATED_BODY()
	
public:
	static UE_API UCommonGameDialogDescriptor* CreateConfirmationOk(const FText& Header, const FText& Body);
	static UE_API UCommonGameDialogDescriptor* CreateConfirmationOkCancel(const FText& Header, const FText& Body);
	static UE_API UCommonGameDialogDescriptor* CreateConfirmationYesNo(const FText& Header, const FText& Body);
	static UE_API UCommonGameDialogDescriptor* CreateConfirmationYesNoCancel(const FText& Header, const FText& Body);

public:
	/** The header of the message to display */
	/** 显示的消息的标题 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Header;
	
	/** The body of the message to display */
	/** 将要显示的消息主体 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Body;

	/** The confirm button's input action to use. */
	/** 确认按钮的输入操作方式。*/
	UPROPERTY(BlueprintReadWrite)
	TArray<FConfirmationDialogAction> ButtonActions;
};


UCLASS(MinimalAPI, Abstract)
class UCommonGameDialog : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	UE_API UCommonGameDialog();
	
	UE_API virtual void SetupDialog(UCommonGameDialogDescriptor* Descriptor, FCommonMessagingResultDelegate ResultCallback);

	UE_API virtual void KillDialog();
};

#undef UE_API
