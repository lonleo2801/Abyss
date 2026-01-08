// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "LoadingProcessInterface.generated.h"

#define UE_API COMMONLOADINGSCREEN_API

/** Interface for things that might cause loading to happen which requires a loading screen to be displayed */
/** 用于表示那些可能会触发加载操作（且该操作需要显示加载界面）的接口 */
UINTERFACE(MinimalAPI, BlueprintType)
class ULoadingProcessInterface : public UInterface
{
	GENERATED_BODY()
};

class ILoadingProcessInterface
{
	GENERATED_BODY()

public:
	// Checks to see if this object implements the interface, and if so asks whether or not we should
	// be currently showing a loading screen
	// 检查该对象是否实现了该接口，如果实现了则询问是否当前应显示加载界面
	static UE_API bool ShouldShowLoadingScreen(UObject* TestObject, FString& OutReason);

	virtual bool ShouldShowLoadingScreen(FString& OutReason) const
	{
		return false;
	}
};

#undef UE_API
