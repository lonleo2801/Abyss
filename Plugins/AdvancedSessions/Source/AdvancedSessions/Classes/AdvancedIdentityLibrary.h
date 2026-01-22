// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineUserInterface.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"

#include "UObject/UObjectIterator.h"

#include "AdvancedIdentityLibrary.generated.h"


// 一般高级会话日志
DECLARE_LOG_CATEGORY_EXTERN(AdvancedIdentityLog, Log, All);

UCLASS()
class UAdvancedIdentityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//********* 身份函数 *************//

	// 获取本地玩家的登录状态
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedIdentity", meta = (ExpandEnumAsExecs = "Result", WorldContext = "WorldContextObject"))
	static void GetLoginStatus(UObject* WorldContextObject, const FBPUniqueNetId & UniqueNetID, EBPLoginStatus & LoginStatus, EBlueprintResultSwitch &Result);

	// 获取本地玩家的认证令牌
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedIdentity", meta = (ExpandEnumAsExecs = "Result", WorldContext = "WorldContextObject"))
	static void GetPlayerAuthToken(UObject* WorldContextObject, APlayerController * PlayerController, FString & AuthToken, EBlueprintResultSwitch &Result);

	// 获取玩家昵称
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedIdentity", meta = (WorldContext = "WorldContextObject"))
	static void GetPlayerNickname(UObject* WorldContextObject, const FBPUniqueNetId & UniqueNetID, FString & PlayerNickname);

	//********* 用户账户函数 *************//

	// 获取用户账户
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedIdentity|UserAccount", meta = (ExpandEnumAsExecs = "Result", WorldContext = "WorldContextObject"))
	static void GetUserAccount(UObject* WorldContextObject, const FBPUniqueNetId & UniqueNetId, FBPUserOnlineAccount & AccountInfo, EBlueprintResultSwitch &Result);

	// 获取所有已知用户账户
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedIdentity|UserAccount", meta = (ExpandEnumAsExecs = "Result", WorldContext = "WorldContextObject"))
	static void GetAllUserAccounts(UObject* WorldContextObject, TArray<FBPUserOnlineAccount> & AccountInfos, EBlueprintResultSwitch &Result);

	// 获取用户账户访问令牌
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedIdentity|UserAccount")
	static void GetUserAccountAccessToken(const FBPUserOnlineAccount & AccountInfo, FString & AccessToken);

	// 获取用户账户认证属性（取决于子系统）
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedIdentity|UserAccount", meta = (ExpandEnumAsExecs = "Result"))
	static void GetUserAccountAuthAttribute(const FBPUserOnlineAccount & AccountInfo, const FString & AttributeName, FString & AuthAttribute, EBlueprintResultSwitch &Result);

	// 设置用户账户属性（取决于子系统）
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedIdentity|UserAccount", meta = (ExpandEnumAsExecs = "Result"))
	static void SetUserAccountAttribute(const FBPUserOnlineAccount & AccountInfo, const FString & AttributeName, const FString & NewAttributeValue, EBlueprintResultSwitch &Result);

	// 获取用户ID
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedIdentity|UserAccount")
	static void GetUserID(const FBPUserOnlineAccount & AccountInfo, FBPUniqueNetId & UniqueNetID);

	// 获取用户账户真实姓名（如果可能）
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedIdentity|UserAccount")
	static void GetUserAccountRealName(const FBPUserOnlineAccount & AccountInfo, FString & UserName);

	// 获取用户账户显示名称（如果可能）
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedIdentity|UserAccount")
	static void GetUserAccountDisplayName(const FBPUserOnlineAccount & AccountInfo, FString & DisplayName);

	// 获取用户账户属性（取决于子系统）
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedIdentity|UserAccount", meta = (ExpandEnumAsExecs = "Result"))
	static void GetUserAccountAttribute(const FBPUserOnlineAccount & AccountInfo, const FString & AttributeName, FString & AttributeValue, EBlueprintResultSwitch &Result);


};	
