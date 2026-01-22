// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Online.h"
#include "Engine/LocalPlayer.h"
#include "OnlineSubsystem.h"
#include "BlueprintDataDefinitions.h"
//#include "OnlineFriendsInterface.h"
//#include "OnlineUserInterface.h"
//#include "OnlineMessageInterface.h"
//#include "OnlinePresenceInterface.h"
//#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"

//#include "UObjectIterator.h"

#include "AdvancedExternalUILibrary.generated.h"


// 一般高级会话日志
DECLARE_LOG_CATEGORY_EXTERN(AdvancedExternalUILog, Log, All);

UCLASS()
class UAdvancedExternalUILibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	//********* 外部UI函数 *************//

	// 显示处理好友列表的UI
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedExternalUI", meta = (ExpandEnumAsExecs = "Result", WorldContext = "WorldContextObject"))
	static void ShowFriendsUI(UObject* WorldContextObject, APlayerController *PlayerController, EBlueprintResultSwitch &Result);

	// 显示处理邀请人加入游戏的UI
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedExternalUI", meta = (ExpandEnumAsExecs = "Result", WorldContext = "WorldContextObject"))
	static void ShowInviteUI(UObject* WorldContextObject, APlayerController *PlayerController, EBlueprintResultSwitch &Result);

	// 显示排行榜UI（Steam不支持）
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedExternalUI", meta = (ExpandEnumAsExecs = "Result", WorldContext = "WorldContextObject"))
	static void ShowLeaderBoardUI(UObject* WorldContextObject, FString LeaderboardName, EBlueprintResultSwitch &Result);

	// 显示网页UI
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedExternalUI", meta = (ExpandEnumAsExecs = "Result", AutoCreateRefTerm = "AllowedDomains", WorldContext = "WorldContextObject"))
	static void ShowWebURLUI(UObject* WorldContextObject, FString URLToShow, EBlueprintResultSwitch &Result, TArray<FString>& AllowedDomains, bool bEmbedded = false , bool bShowBackground = false, bool bShowCloseButton = false, int32 OffsetX = 0, int32 OffsetY = 0, int32 SizeX = 0, int32 SizeY = 0);

	// 关闭网页UI
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedExternalUI", meta = (WorldContext = "WorldContextObject"))
	static void CloseWebURLUI(UObject* WorldContextObject);


	// 显示唯一网络ID的个人资料UI
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedExternalUI", meta = (ExpandEnumAsExecs = "Result", WorldContext = "WorldContextObject"))
	static void ShowProfileUI(UObject* WorldContextObject, const FBPUniqueNetId PlayerViewingProfile, const FBPUniqueNetId PlayerToViewProfileOf, EBlueprintResultSwitch &Result);

	// 显示账户升级UI（Steam不支持）
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedExternalUI", meta = (ExpandEnumAsExecs = "Result", WorldContext = "WorldContextObject"))
	static void ShowAccountUpgradeUI(UObject* WorldContextObject, const FBPUniqueNetId PlayerRequestingAccountUpgradeUI, EBlueprintResultSwitch &Result);

};	
