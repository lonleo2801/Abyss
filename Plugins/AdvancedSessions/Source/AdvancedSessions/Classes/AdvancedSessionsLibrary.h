// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "BlueprintDataDefinitions.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineUserInterface.h"
#include "Interfaces/OnlineMessageInterface.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"

//#include "UObjectIterator.h"

#include "AdvancedSessionsLibrary.generated.h"


// 一般高级会话日志
DECLARE_LOG_CATEGORY_EXTERN(AdvancedSessionsLog, Log, All);


UCLASS()
class UAdvancedSessionsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
		//********* 会话管理函数 *************//	

		// 从当前活跃的游戏会话中踢出一名玩家，仅在服务器上可用
		UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSessions", meta = (WorldContext = "WorldContextObject"))
		static bool KickPlayer(UObject* WorldContextObject, APlayerController* PlayerToKick, FText KickReason);

		// 禁止一名玩家进入当前活跃的游戏会话，仅在服务器上可用
		// 注意：默认的 gamesession 类并没有实现实际的封禁列表，调用此函数时仅会执行踢出操作
		UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSessions", meta = (WorldContext = "WorldContextObject"))
		static bool BanPlayer(UObject* WorldContextObject, APlayerController* PlayerToBan, FText BanReason);

		//********* 会话搜索函数 *************//	

		// 根据设置是否存在，向现有数组添加或修改会话设置
		UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSessions|SessionInfo")
		static void AddOrModifyExtraSettings(UPARAM(ref)  TArray<FSessionPropertyKeyPair> & SettingsArray, UPARAM(ref)  TArray<FSessionPropertyKeyPair> & NewOrChangedSettings, TArray<FSessionPropertyKeyPair> & ModifiedSettingsArray);

		// 从会话搜索结果中获取会话设置数组
		UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSessions|SessionInfo")
		static void GetExtraSettings(FBlueprintSessionResult SessionResult, TArray<FSessionPropertyKeyPair> & ExtraSettings);

		// 获取当前会话状态
		UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSessions|SessionInfo", meta = (WorldContext = "WorldContextObject"))
		static void GetSessionState(UObject* WorldContextObject, EBPOnlineSessionState &SessionState);

		// 获取当前会话设置
		UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSessions|SessionInfo", meta = (ExpandEnumAsExecs = "Result", WorldContext = "WorldContextObject"))
		static void GetSessionSettings(UObject* WorldContextObject, int32 &NumConnections, int32 &NumPrivateConnections, bool &bIsLAN, bool &bIsDedicated, bool &bAllowInvites, bool &bAllowJoinInProgress, bool &bIsAnticheatEnabled, int32 &BuildUniqueID, TArray<FSessionPropertyKeyPair> &ExtraSettings, EBlueprintResultSwitch &Result);

		// 检查某人是否在当前会话中
		UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSessions|SessionInfo", meta = (WorldContext = "WorldContextObject"))
		static void IsPlayerInSession(UObject* WorldContextObject, const FBPUniqueNetId &PlayerToCheck, bool &bIsInSession);
		
		// 制作一个字面量的会话搜索参数
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|SessionInfo|Literals")
		static FSessionsSearchSetting MakeLiteralSessionSearchProperty(FSessionPropertyKeyPair SessionSearchProperty, EOnlineComparisonOpRedux ComparisonOp);


		//********* 会话信息函数 ***********//

		// 检查会话结果是否有效
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|SessionInfo")
		static bool IsValidSession(const FBlueprintSessionResult & SessionResult);

		// 获取会话ID的字符串副本
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|SessionInfo")
		static void GetSessionID_AsString(const FBlueprintSessionResult & SessionResult, FString& SessionID);

		// 获取当前会话ID的字符串副本
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|SessionInfo", meta = (WorldContext = "WorldContextObject"))
		static void GetCurrentSessionID_AsString(UObject* WorldContextObject, FString& SessionID);

		// 获取唯一当前构建ID
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|SessionInfo")
		static void GetCurrentUniqueBuildID(int32 &UniqueBuildId);
		
		// 从会话搜索结果中获取唯一构建ID
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|SessionInfo")
		static void GetUniqueBuildID(FBlueprintSessionResult SessionResult, int32 &UniqueBuildId);
		
		
		// 感谢 CriErr 的提交


		// 获取会话属性键名值
		UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSessions|SessionInfo")
		static FName GetSessionPropertyKey(const FSessionPropertyKeyPair& SessionProperty);
		
		// 按名称查找会话属性
		UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSessions|SessionInfo", meta = (ExpandEnumAsExecs = "Result"))
		static void FindSessionPropertyByName(const TArray<FSessionPropertyKeyPair>& ExtraSettings, FName SettingsName, EBlueprintResultSwitch &Result, FSessionPropertyKeyPair& OutProperty);
		
		// 按名称查找会话属性索引
		UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSessions|SessionInfo", meta = (ExpandEnumAsExecs = "Result"))
		static void FindSessionPropertyIndexByName(const TArray<FSessionPropertyKeyPair>& ExtraSettings, FName SettingName, EBlueprintResultSwitch &Result, int32& OutIndex);

		/// 移除了最后一个函数的 Index_None 部分，蓝图中无法访问，最好返回成功/失败
		// 结束感谢 CriErr :p

		// 获取会话自定义信息键/值作为字节（用于枚举）
		UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSessions|SessionInfo", meta = (ExpandEnumAsExecs = "SearchResult"))
		static void GetSessionPropertyByte(const TArray<FSessionPropertyKeyPair> & ExtraSettings, FName SettingName, ESessionSettingSearchResult &SearchResult, uint8 &SettingValue);

		// 获取会话自定义信息键/值作为布尔值
		// Steam 目前仅支持 Int,Float,String,BYTE 值进行搜索过滤！！！
		UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSessions|SessionInfo", meta = (ExpandEnumAsExecs = "SearchResult"))
		static void GetSessionPropertyBool(const TArray<FSessionPropertyKeyPair> & ExtraSettings, FName SettingName, ESessionSettingSearchResult &SearchResult, bool &SettingValue);

		// 获取会话自定义信息键/值作为字符串
		UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSessions|SessionInfo", meta = (ExpandEnumAsExecs = "SearchResult"))
		static void GetSessionPropertyString(const TArray<FSessionPropertyKeyPair> & ExtraSettings, FName SettingName, ESessionSettingSearchResult &SearchResult, FString &SettingValue);

		// 获取会话自定义信息键/值作为整数
		UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSessions|SessionInfo", meta = (ExpandEnumAsExecs = "SearchResult"))
		static void GetSessionPropertyInt(const TArray<FSessionPropertyKeyPair> & ExtraSettings, FName SettingName, ESessionSettingSearchResult &SearchResult, int32 &SettingValue);

		// 获取会话自定义信息键/值作为浮点数
		UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSessions|SessionInfo", meta = (ExpandEnumAsExecs = "SearchResult"))
		static void GetSessionPropertyFloat(const TArray<FSessionPropertyKeyPair> & ExtraSettings, FName SettingName, ESessionSettingSearchResult &SearchResult, float &SettingValue);


		// 从字节制作一个字面量的会话自定义信息键/值对（用于枚举）
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|SessionInfo|Literals")
		static FSessionPropertyKeyPair MakeLiteralSessionPropertyByte(FName Key, uint8 Value);

		// 从布尔值制作一个字面量的会话自定义信息键/值对
		// Steam 目前仅支持 Int,Float,String,BYTE 值进行搜索过滤！
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|SessionInfo|Literals")
		static FSessionPropertyKeyPair MakeLiteralSessionPropertyBool(FName Key, bool Value);

		// 从字符串制作一个字面量的会话自定义信息键/值对
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|SessionInfo|Literals")
		static FSessionPropertyKeyPair MakeLiteralSessionPropertyString(FName Key, FString Value);

		// 从整数制作一个字面量的会话自定义信息键/值对
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|SessionInfo|Literals")
		static FSessionPropertyKeyPair MakeLiteralSessionPropertyInt(FName Key, int32 Value);

		// 从浮点数制作一个字面量的会话自定义信息键/值对
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|SessionInfo|Literals")
		static FSessionPropertyKeyPair MakeLiteralSessionPropertyFloat(FName Key, float Value);


		//******* 玩家ID函数 *********//

		// 获取附加到给定控制器的网络玩家的唯一网络ID
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|PlayerInfo|PlayerID")
		static void GetUniqueNetID(APlayerController *PlayerController, FBPUniqueNetId &UniqueNetId);

		// 获取会话拥有者的唯一网络ID
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|PlayerInfo|PlayerID")
		static void GetUniqueNetIdOfSessionOwner(FBlueprintSessionResult SessionResult, FBPUniqueNetId& UniqueNetId);

		// 获取分配给给定玩家状态的网络玩家的唯一网络ID
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|PlayerInfo|PlayerID")
		static void GetUniqueNetIDFromPlayerState(APlayerState *PlayerState, FBPUniqueNetId &UniqueNetId);
		
		// 如果唯一网络ID有效则返回 True
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|PlayerInfo|PlayerID")
		static bool IsValidUniqueNetID(const FBPUniqueNetId &UniqueNetId);

		/* 如果值相等则返回 true (A == B) */
		UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal Unique Net ID", CompactNodeTitle = "==", Keywords = "== equal"), Category = "Online|AdvancedSessions|PlayerInfo|PlayerID")
		static bool EqualEqual_UNetIDUnetID(const FBPUniqueNetId &A, const FBPUniqueNetId &B);

		/** 将 FBPUniqueNetID 转换为 FUniqueNetIdRepl */
		UFUNCTION(BlueprintPure, meta = (DisplayName = "ToUniqueNetIDRepl (Unique Net ID)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Online|AdvancedSessions|PlayerInfo|PlayerID")
			static FUniqueNetIdRepl Conv_BPUniqueIDToUniqueNetIDRepl(const FBPUniqueNetId& InUniqueID);

		// 将 UniqueNetId 转换为字符串
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|UniqueNetId")
		static void UniqueNetIdToString(const FBPUniqueNetId &UniqueNetId, FString &String);

		//******** 玩家名称函数 **********//

		// 获取附加到给定控制器的网络玩家的玩家名称
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|PlayerInfo|PlayerName")
		static void GetPlayerName(APlayerController *PlayerController, FString &PlayerName);

		// 设置附加到给定控制器的网络玩家的玩家名称
		UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSessions|PlayerInfo|PlayerName")
		static void SetPlayerName(APlayerController *PlayerController, FString PlayerName);

		//********** 杂项玩家信息函数 *********//

		// 获取网络玩家数量
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|PlayerInfo|Misc", meta = (bIgnoreSelf = "true", WorldContext = "WorldContextObject", DisplayName = "GetNumNetworkPlayers"))
		static void GetNumberOfNetworkPlayers(UObject* WorldContextObject, int32 &NumNetPlayers);

		// 获取给定控制器的网络玩家索引
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|PlayerInfo|Misc")
		static void GetNetPlayerIndex(APlayerController *PlayerController, int32 &NetPlayerIndex);

		// 检查所述会话子系统是否处于活动状态
		UFUNCTION(BlueprintPure, Category = "Online|AdvancedSessions|Misc")
		static bool HasOnlineSubsystem(FName SubSystemName);

		//**** 无缝漫游函数 ****//

		// 将服务器漫游暴露给蓝图
		UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Online|AdvancedSessions|Seamless", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool ServerTravel(UObject* WorldContextObject, const FString& InURL, bool bAbsolute, bool bShouldSkipGameNotify);
		
};	
