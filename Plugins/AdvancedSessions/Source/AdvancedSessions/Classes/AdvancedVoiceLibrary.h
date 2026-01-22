// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlueprintDataDefinitions.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "Interfaces/VoiceInterface.h"
//#include "OnlineFriendsInterface.h"
//#include "OnlineUserInterface.h"
//#include "OnlineMessageInterface.h"
//#include "OnlinePresenceInterface.h"
#include "Engine/GameInstance.h"
//#include "OnlineSessionInterface.h"

#include "UObject/UObjectIterator.h"

#include "AdvancedVoiceLibrary.generated.h"


// 一般高级会话日志
DECLARE_LOG_CATEGORY_EXTERN(AdvancedVoiceLog, Log, All);


UCLASS()
class UAdvancedVoiceLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	//********* 语音库函数 *************//

	// 获取指定本地用户是否存在耳机
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedVoice|VoiceInfo", meta = (WorldContext = "WorldContextObject"))
	static void IsHeadsetPresent(UObject* WorldContextObject, bool & bHasHeadset, uint8 LocalPlayerNum = 0);

	// 启动网络语音，配合 StopNetworkedVoice 允许按键通话
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice", meta = (WorldContext = "WorldContextObject"))
	static void StartNetworkedVoice(UObject* WorldContextObject, uint8 LocalPlayerNum = 0);

	// 停止网络语音，配合 StartNetworkedVoice 允许按键通话
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice", meta = (WorldContext = "WorldContextObject"))
	static void StopNetworkedVoice(UObject* WorldContextObject, uint8 LocalPlayerNum = 0);

	// 注册本地玩家为对语音数据感兴趣的人
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice", meta = (WorldContext = "WorldContextObject"))
	static bool RegisterLocalTalker(UObject* WorldContextObject, uint8 LocalPlayerNum = 0);

	// 将所有已登录玩家注册为本地发言者
	// 这通常是自动完成的，只有在注销某人后才手动执行此操作
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice", meta = (WorldContext = "WorldContextObject"))
	static void RegisterAllLocalTalkers(UObject* WorldContextObject);

	// 注销本地玩家的本地发言者身份
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice", meta = (WorldContext = "WorldContextObject"))
	static void UnRegisterLocalTalker(UObject* WorldContextObject, uint8 LocalPlayerNum = 0);

	// 注销所有已登录玩家的本地发言者身份
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice", meta = (WorldContext = "WorldContextObject"))
	static void UnRegisterAllLocalTalkers(UObject* WorldContextObject);

	// 注册远程玩家为发言者
	// 这通常是自动完成的，只有在注销某人后才手动执行此操作
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice", meta = (WorldContext = "WorldContextObject"))
	static bool RegisterRemoteTalker(UObject* WorldContextObject, const FBPUniqueNetId& UniqueNetId);

	// 注销远程玩家的发言者身份
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice", meta = (WorldContext = "WorldContextObject"))
	static bool UnRegisterRemoteTalker(UObject* WorldContextObject, const FBPUniqueNetId& UniqueNetId);

	// 注销所有远程玩家的发言者身份
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice", meta = (WorldContext = "WorldContextObject"))
	static void RemoveAllRemoteTalkers(UObject* WorldContextObject);

	// 返回本地玩家是否正在说话
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedVoice|VoiceInfo", meta = (WorldContext = "WorldContextObject"))
	static bool IsLocalPlayerTalking(UObject* WorldContextObject, uint8 LocalPlayerNum);

	// 返回远程玩家是否正在说话
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedVoice|VoiceInfo", meta = (WorldContext = "WorldContextObject"))
	static bool IsRemotePlayerTalking(UObject* WorldContextObject, const FBPUniqueNetId& UniqueNetId);

	// 返回玩家是否对指定的本地玩家静音
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedVoice|VoiceInfo", meta = (WorldContext = "WorldContextObject"))
	static bool IsPlayerMuted(UObject* WorldContextObject, uint8 LocalUserNumChecking, const FBPUniqueNetId& UniqueNetId);

	// 为指定的本地玩家静音与 uniquenetid 关联的玩家，如果 IsSystemWide 为真，则尝试对该玩家进行全局静音
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice", meta = (WorldContext = "WorldContextObject"))
	static bool MuteRemoteTalker(UObject* WorldContextObject, uint8 LocalUserNum, const FBPUniqueNetId& UniqueNetId, bool bIsSystemWide = false);

	// 为指定的本地玩家取消静音与 uniquenetid 关联的玩家，如果 IsSystemWide 为真，则尝试对该玩家进行全局取消静音
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice", meta = (WorldContext = "WorldContextObject"))
	static bool UnMuteRemoteTalker(UObject* WorldContextObject, uint8 LocalUserNum, const FBPUniqueNetId& UniqueNetId, bool bIsSystemWide = false);

	// 获取此系统的本地发言者数量
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedVoice|VoiceInfo", meta = (WorldContext = "WorldContextObject"))
	static void GetNumLocalTalkers(UObject* WorldContextObject, int32 & NumLocalTalkers);
};	
