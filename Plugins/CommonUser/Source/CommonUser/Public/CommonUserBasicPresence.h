// Copyright Epic Games, Inc. All Rights Reserved.
// Finished.
#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "CommonUserBasicPresence.generated.h"

#define UE_API COMMONUSER_API

class UCommonSessionSubsystem;
enum class ECommonSessionInformationState : uint8;

//////////////////////////////////////////////////////////////////////
// UCommonUserBasicPresence

/**
 * This subsystem plugs into the session subsystem and pushes its information to the presence interface.
 * It is not intended to be a full featured rich presence implementation, but can be used as a proof-of-concept
 * for pushing information from the session subsystem to the presence system
 *
 * 该子系统与会话子系统相连接，并将自身信息推送到状态显示界面。
 * 它并非旨在成为功能完备的丰富状态显示实现，但可以作为将会话子系统中的信息推送到状态显示系统的一个概念验证示例。
 */
UCLASS(MinimalAPI, BlueprintType, Config = Engine)
class UCommonUserBasicPresence : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UE_API UCommonUserBasicPresence();


	/** Implement this for initialization of instances of the system */
	/** 请实现此功能以对系统实例进行初始化 */
	UE_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	/** 为系统实例的销毁操作实现此功能 */
	UE_API virtual void Deinitialize() override;

	/** False is a general purpose killswitch to stop this class from pushing presence*/
	/** 假值是一个通用的终止开关，用于阻止此类发送状态信息 */
	UPROPERTY(Config)
	bool bEnableSessionsBasedPresence = false;

	/** Maps the presence status "In-game" to a backend key*/
	/** 将“游戏内”这一在线状态映射为后端的某个键值 */
	UPROPERTY(Config)
	FString PresenceStatusInGame;

	/** Maps the presence status "Main Menu" to a backend key*/
	/** 将“主菜单”这一显示状态映射到后端的一个键值上 */
	UPROPERTY(Config)
	FString PresenceStatusMainMenu;

	/** Maps the presence status "Matchmaking" to a backend key*/
	/** 将“匹配中”这一在线状态映射为后端的标识键 */
	UPROPERTY(Config)
	FString PresenceStatusMatchmaking;

	/** Maps the "Game Mode" rich presence entry to a backend key*/
	/** 将“游戏模式”丰富的状态提示项映射到后端的键值上 */
	UPROPERTY(Config)
	FString PresenceKeyGameMode;

	/** Maps the "Map Name" rich presence entry to a backend key*/
	/** 将“地图名称”这一丰富的状态信息条目映射到后端的某个键上 */
	UPROPERTY(Config)
	FString PresenceKeyMapName;

	UE_API void OnNotifySessionInformationChanged(ECommonSessionInformationState SessionStatus, const FString& GameMode, const FString& MapName);
	UE_API FString SessionStateToBackendKey(ECommonSessionInformationState SessionStatus);
};

#undef UE_API
