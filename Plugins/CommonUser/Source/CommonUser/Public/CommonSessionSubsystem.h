// Copyright Epic Games, Inc. All Rights Reserved.
// Finished.
#pragma once

#include "CommonUserTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/ObjectPtr.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/PrimaryAssetId.h"
#include "UObject/WeakObjectPtr.h"
#include "PartyBeaconClient.h"
#include "PartyBeaconHost.h"
#include "PartyBeaconState.h"
#if! COMMONUSER_OSSV1
#include "Online/Sessions.h"
#endif



class APlayerController;
class AOnlineBeaconHost;
class ULocalPlayer;
namespace ETravelFailure { enum Type : int; }
struct FOnlineResultInformation;

#if COMMONUSER_OSSV1
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#else
#include "Online/Lobbies.h"
#include "Online/OnlineAsyncOpHandle.h"
#endif // COMMONUSER_OSSV1

#include "CommonSessionSubsystem.generated.h"

class UWorld;
class FCommonSession_OnlineSessionSettings;

#if COMMONUSER_OSSV1
class FCommonOnlineSearchSettingsOSSv1;
using FCommonOnlineSearchSettings = FCommonOnlineSearchSettingsOSSv1;
#else
class FCommonOnlineSearchSettingsOSSv2;
using FCommonOnlineSearchSettings = FCommonOnlineSearchSettingsOSSv2;
#endif // COMMONUSER_OSSV1


//////////////////////////////////////////////////////////////////////
// UCommonSession_HostSessionRequest

/** Specifies the online features and connectivity that should be used for a game session */
/** 指定游戏会话中应采用的在线功能和连接方式 */
UENUM(BlueprintType)
enum class ECommonSessionOnlineMode : uint8
{
	Offline,
	LAN,
	Online
};

/** A request object that stores the parameters used when hosting a gameplay session */
/** 一个请求对象，用于存储举办游戏环节时所使用的参数 */
UCLASS(MinimalAPI, BlueprintType)
class UCommonSession_HostSessionRequest : public UObject
{
	GENERATED_BODY()

public:
	/** Indicates if the session is a full online session or a different type */
	/** 表示该会话是完整的在线会话还是其他类型的会话 */
	UPROPERTY(BlueprintReadWrite, Category=Session)
	ECommonSessionOnlineMode OnlineMode;

	/** True if this request should create a player-hosted lobbies if available */
	/** 如果当前请求能够在可用的情况下创建玩家主导型的游戏房间，则返回 true */
	UPROPERTY(BlueprintReadWrite, Category = Session)
	bool bUseLobbies;

	/** True if this request should create a lobby with enabled voice chat in available */
	/** 如果此请求需要创建一个具备启用语音聊天功能的房间，则返回 true */
	UPROPERTY(BlueprintReadWrite, Category = Session)
	bool bUseLobbiesVoiceChat;

	/** True if this request should create a session that will appear in the user's presence information */
	/** 如果此请求需要创建一个会话，并且该会话将出现在用户的个人资料中，则返回 true */
	UPROPERTY(BlueprintReadWrite, Category = Session)
	bool bUsePresence;

	/** String used during matchmaking to specify what type of game mode this is */
	/** 在匹配过程中使用的字符串，用于指定此游戏模式的类型 */
	UPROPERTY(BlueprintReadWrite, Category=Session)
	FString ModeNameForAdvertisement;

	/** The map that will be loaded at the start of gameplay, this needs to be a valid Primary Asset top-level map */
	/** 游戏开始时将加载的地图，此地图必须是有效的主资产层级地图 */
	UPROPERTY(BlueprintReadWrite, Category=Session, meta=(AllowedTypes="World"))
	FPrimaryAssetId MapID;

	/** Extra arguments passed as URL options to the game */
	/** 作为游戏的 URL 选项传递的额外参数 */
	UPROPERTY(BlueprintReadWrite, Category=Session)
	TMap<FString, FString> ExtraArgs;

	/** Maximum players allowed per gameplay session */
	/** 每次游戏时段允许的最大玩家数量 */
	UPROPERTY(BlueprintReadWrite, Category=Session)
	int32 MaxPlayerCount = 16;

public:
	/** Returns the maximum players that should actually be used, could be overridden in child classes */
	/** 返回实际应使用的最大玩家数量，该值可在子类中进行重写 */
	COMMONUSER_API virtual int32 GetMaxPlayers() const;

	/** Returns the full map name that will be used during gameplay */
	/** 返回在游戏过程中将使用的完整地图名称 */
	COMMONUSER_API virtual FString GetMapName() const;

	/** Constructs the full URL that will be passed to ServerTravel */
	/** 构建将传递给“服务器导航”功能的完整 URL */
	COMMONUSER_API virtual FString ConstructTravelURL() const;

	/** Returns true if this request is valid, returns false and logs errors if it is not */
	/** 如果此请求有效则返回 true，否则返回 false 并记录错误信息 */
	COMMONUSER_API virtual bool ValidateAndLogErrors(FText& OutError) const;
};


//////////////////////////////////////////////////////////////////////
// UCommonSession_SearchResult

/** A result object returned from the online system that describes a joinable game session */
/** 从在线系统返回的结果对象，该对象描述了一个可参与的游戏时段 */
UCLASS(MinimalAPI, BlueprintType)
class UCommonSession_SearchResult : public UObject
{
	GENERATED_BODY()

public:
	/** Returns an internal description of the session, not meant to be human readable */
	/** 返回会话的内部描述，此描述不供人类阅读 */
	UFUNCTION(BlueprintCallable, Category=Session)
	COMMONUSER_API FString GetDescription() const;

	/** Gets an arbitrary string setting, bFoundValue will be false if the setting does not exist */
	/** 获取任意字符串设置，若该设置不存在，则变量 bFoundValue 的值将为 false */
	UFUNCTION(BlueprintPure, Category=Sessions)
	COMMONUSER_API void GetStringSetting(FName Key, FString& Value, bool& bFoundValue) const;

	/** Gets an arbitrary integer setting, bFoundValue will be false if the setting does not exist */
	/** 获取任意整数值设置，若该设置不存在，则变量 bFoundValue 的值为 false */
	UFUNCTION(BlueprintPure, Category = Sessions)
	COMMONUSER_API void GetIntSetting(FName Key, int32& Value, bool& bFoundValue) const;

	/** The number of private connections that are available */
	/** 当前可用的私有连接数量 */
	UFUNCTION(BlueprintPure, Category=Sessions)
	COMMONUSER_API int32 GetNumOpenPrivateConnections() const;

	/** The number of publicly available connections that are available */
	/** 公开可用的连接数量 */
	UFUNCTION(BlueprintPure, Category=Sessions)
	COMMONUSER_API int32 GetNumOpenPublicConnections() const;

	/** The maximum number of publicly available connections that could be available, including already filled connections */
	/** 可供公众使用的最大连接数量，包括已占用的连接数量 */
	UFUNCTION(BlueprintPure, Category = Sessions)
	COMMONUSER_API int32 GetMaxPublicConnections() const;

	/** Ping to the search result, MAX_QUERY_PING is unreachable */
	/** 向搜索结果发送 Ping 操作，但 MAX_QUERY_PING 无法到达 */
	UFUNCTION(BlueprintPure, Category=Sessions)
	COMMONUSER_API int32 GetPingInMs() const;

public:
	/** Pointer to the platform-specific implementation */
	/** 指向平台特定实现的指针 */
#if COMMONUSER_OSSV1
	FOnlineSessionSearchResult Result;
#else
	TSharedPtr<const UE::Online::FLobby> Lobby;

	UE::Online::FOnlineSessionId SessionID;
#endif // COMMONUSER_OSSV1

};


//////////////////////////////////////////////////////////////////////
// UCommonSession_SearchSessionRequest

/** Delegates called when a session search completes */
/** 当会话搜索完成时所调用的回调函数 */
DECLARE_MULTICAST_DELEGATE_TwoParams(FCommonSession_FindSessionsFinished, bool bSucceeded, const FText& ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCommonSession_FindSessionsFinishedDynamic, bool, bSucceeded, FText, ErrorMessage);

/** Request object describing a session search, this object will be updated once the search has completed */
/** 描述会话搜索的请求对象，该对象在搜索完成后会进行更新 */
UCLASS(MinimalAPI, BlueprintType)
class UCommonSession_SearchSessionRequest : public UObject
{
	GENERATED_BODY()

public:
	/** Indicates if the this is looking for full online games or a different type like LAN */
	/** 表示此功能是用于查找完整的在线游戏，还是查找诸如局域网游戏之类的其他类型的游戏 */
	UPROPERTY(BlueprintReadWrite, Category = Session)
	ECommonSessionOnlineMode OnlineMode;

	/** True if this request should look for player-hosted lobbies if they are available, false will only search for registered server sessions */
	/** 如果存在玩家自建的房间，则应执行此请求以查找这些房间；若为假，则仅搜索已注册的服务器会话 */
	UPROPERTY(BlueprintReadWrite, Category = Session)
	bool bUseLobbies;

	/** List of all found sessions, will be valid when OnSearchFinished is called */
	/** 所有找到的会话列表，当调用“OnSearchFinished”时该列表仍有效 */
	UPROPERTY(BlueprintReadOnly, Category=Session)
	TArray<TObjectPtr<UCommonSession_SearchResult>> Results;

	/** Native Delegate called when a session search completes */
	/** 当会话搜索完成时所调用的原生委托函数 */
	FCommonSession_FindSessionsFinished OnSearchFinished;

	/** Called by subsystem to execute finished delegates */
	/** 由子系统调用以执行已完成的委托 */
	COMMONUSER_API void NotifySearchFinished(bool bSucceeded, const FText& ErrorMessage);

private:
	/** Delegate called when a session search completes */
	/** 当会话搜索完成时所调用的委托函数 */
	UPROPERTY(BlueprintAssignable, Category = "Events", meta = (DisplayName = "On Search Finished", AllowPrivateAccess = true))
	FCommonSession_FindSessionsFinishedDynamic K2_OnSearchFinished;
};


//////////////////////////////////////////////////////////////////////
// CommonSessionSubsystem Events

/**
 * Event triggered when the local user has requested to join a session from an external source, for example from a platform overlay.
 * Generally, the game should transition the player into the session.
 * @param LocalPlatformUserId the local user id that accepted the invitation. This is a platform user id because the user might not be signed in yet.
 * @param RequestedSession the requested session. Can be null if there was an error processing the request.
 * @param RequestedSessionResult result of the requested session processing
 */
/**
* 当本地用户从外部来源（例如平台覆盖层）请求加入会话时触发此事件。
* 通常，游戏应将玩家引导进入该会话。
* @参数 LocalPlatformUserId 接受邀请的本地用户 ID。这是一个平台用户 ID，因为用户可能尚未登录。
* @参数 RequestedSession 被请求的会话。如果处理请求时出现错误，则可以为 null。
* @参数 RequestedSessionResult 请求会话处理的结果*/
DECLARE_MULTICAST_DELEGATE_ThreeParams(FCommonSessionOnUserRequestedSession, const FPlatformUserId& /*LocalPlatformUserId*/, UCommonSession_SearchResult* /*RequestedSession*/, const FOnlineResultInformation& /*RequestedSessionResult*/);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCommonSessionOnUserRequestedSession_Dynamic, const FPlatformUserId&, LocalPlatformUserId, UCommonSession_SearchResult*, RequestedSession, const FOnlineResultInformation&, RequestedSessionResult);

/**
 * Event triggered when a session join has completed, after joining the underlying session and before traveling to the server if it was successful.
 * The event parameters indicate if this was successful, or if there was an error that will stop it from traveling.
 * @param Result result of the session join
 */
/**
* 当会话加入操作完成时触发此事件，即在加入底层会话之后且在前往服务器之前（如果操作成功的话）。
* 该事件参数表明此次操作是否成功，或者是否存在会导致无法前往服务器的错误。
* @参数 结果：会话加入的结果*/
DECLARE_MULTICAST_DELEGATE_OneParam(FCommonSessionOnJoinSessionComplete, const FOnlineResultInformation& /*Result*/);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommonSessionOnJoinSessionComplete_Dynamic, const FOnlineResultInformation&, Result);

/**
 * Event triggered when a session creation for hosting has completed, right before it travels to the map.
 * The event parameters indicate if this was successful, or if there was an error that will stop it from traveling.
 * @param Result result of the session join
 */
/**
* 当用于托管的会话创建完成并即将传输至地图时触发此事件。
* 该事件参数表明此次操作是否成功，或者是否存在会导致传输中断的错误。
* @参数 结果：会话加入的结果*/
DECLARE_MULTICAST_DELEGATE_OneParam(FCommonSessionOnCreateSessionComplete, const FOnlineResultInformation& /*Result*/);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommonSessionOnCreateSessionComplete_Dynamic, const FOnlineResultInformation&, Result);

/**
 * Event triggered when the local user has requested to destroy a session from an external source, for example from a platform overlay.
 * The game should transition the player out of the session.
 * @param LocalPlatformUserId the local user id that made the destroy request. This is a platform user id because the user might not be signed in yet.
 * @param SessionName the name identifier for the session.
 */
/**
* 当本地用户从外部来源（例如平台覆盖层）请求销毁会话时触发此事件。
* 游戏应将玩家从该会话中移出。
* @参数 LocalPlatformUserId 发出销毁请求的本地用户 ID。这是一个平台用户 ID，因为用户可能尚未登录。
* @参数 SessionName 会话的名称标识符。*/
DECLARE_MULTICAST_DELEGATE_TwoParams(FCommonSessionOnDestroySessionRequested, const FPlatformUserId& /*LocalPlatformUserId*/, const FName& /*SessionName*/);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCommonSessionOnDestroySessionRequested_Dynamic, const FPlatformUserId&, LocalPlatformUserId, const FName&, SessionName);

/**
 * Event triggered when a session join has completed, after resolving the connect string and prior to the client traveling.
 * @param URL resolved connection string for the session with any additional arguments
 */
/**
* 当会话加入操作完成时触发此事件，此时已解析连接字符串，且客户端尚未开始移动。
* @参数 URL 该会话的已解析连接字符串（包含任何附加参数）*/
DECLARE_MULTICAST_DELEGATE_OneParam(FCommonSessionOnPreClientTravel, FString& /*URL*/);

/**
 * Event triggered at different points in the session ecosystem that represent a user-presentable state of the session.
 * This should not be used for online functionality (use OnCreateSessionComplete or OnJoinSessionComplete for those) but for features such as rich presence
 */
/**
* 在会话生态系统的不同环节触发的事件，这些事件代表了会话的用户可查看的状态。
* 不应将其用于在线功能（应使用 OnCreateSessionComplete 或 OnJoinSessionComplete 来实现此类功能），而应用于诸如丰富状态显示等特性。*/
UENUM(BlueprintType)
enum class ECommonSessionInformationState : uint8
{
	OutOfGame,
	Matchmaking,
	InGame
};
DECLARE_MULTICAST_DELEGATE_ThreeParams(FCommonSessionOnSessionInformationChanged, ECommonSessionInformationState /*SessionStatus*/, const FString& /*GameMode*/, const FString& /*MapName*/);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCommonSessionOnSessionInformationChanged_Dynamic, ECommonSessionInformationState, SessionStatus, const FString&, GameMode, const FString&, MapName);

//////////////////////////////////////////////////////////////////////
// UCommonSessionSubsystem

/** 
 * Game subsystem that handles requests for hosting and joining online games.
 * One subsystem is created for each game instance and can be accessed from blueprints or C++ code.
 * If a game-specific subclass exists, this base subsystem will not be created.
 */
/**
* 负责处理在线游戏的托管和加入请求的游戏子系统。
* 每个游戏实例都会创建一个独立的子系统，并且可以从蓝图或 C++ 代码中访问该子系统。
* 如果存在特定于游戏的子类，则不会创建这个基础子系统。*/
UCLASS(MinimalAPI, BlueprintType, Config=Engine)
class UCommonSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UCommonSessionSubsystem() { }

	COMMONUSER_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	COMMONUSER_API virtual void Deinitialize() override;
	COMMONUSER_API virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/** Creates a host session request with default options for online games, this can be modified after creation */
	/** 创建一个适用于在线游戏的主机会话请求，该请求默认包含相关选项，创建后可对其进行修改 */
	UFUNCTION(BlueprintCallable, Category = Session)
	COMMONUSER_API virtual UCommonSession_HostSessionRequest* CreateOnlineHostSessionRequest();

	/** Creates a session search object with default options to look for default online games, this can be modified after creation */
	/** 创建一个会话搜索对象，使用默认选项以查找默认的在线游戏，创建后可对其进行修改 */
	UFUNCTION(BlueprintCallable, Category = Session)
	COMMONUSER_API virtual UCommonSession_SearchSessionRequest* CreateOnlineSearchSessionRequest();

	/** Creates a new online game using the session request information, if successful this will start a hard map transfer */
	/** 根据会话请求信息创建一个新的在线游戏，如果操作成功，将启动一次硬地图传输 */
	UFUNCTION(BlueprintCallable, Category=Session)
	COMMONUSER_API virtual void HostSession(APlayerController* HostingPlayer, UCommonSession_HostSessionRequest* Request);

	/** Starts a process to look for existing sessions or create a new one if no viable sessions are found */
	/** 启动一个进程，以查找现有的会话，若未找到可用的会话则创建一个新的会话 */
	UFUNCTION(BlueprintCallable, Category=Session)
	COMMONUSER_API virtual void QuickPlaySession(APlayerController* JoiningOrHostingPlayer, UCommonSession_HostSessionRequest* Request);

	/** Starts process to join an existing session, if successful this will connect to the specified server */
	/** 启动进程以加入现有会话，如果操作成功，将连接到指定的服务器 */
	UFUNCTION(BlueprintCallable, Category=Session)
	COMMONUSER_API virtual void JoinSession(APlayerController* JoiningPlayer, UCommonSession_SearchResult* Request);

	/** Queries online system for the list of joinable sessions matching the search request */
	/** 通过在线系统查询与搜索请求相匹配的可加入会话的列表 */
	UFUNCTION(BlueprintCallable, Category=Session)
	COMMONUSER_API virtual void FindSessions(APlayerController* SearchingPlayer, UCommonSession_SearchSessionRequest* Request);

	/** Clean up any active sessions, called from cases like returning to the main menu */
	/** 清理所有正在运行的会话，此操作在诸如返回主菜单等情况下会被调用 */
	UFUNCTION(BlueprintCallable, Category=Session)
	COMMONUSER_API virtual void CleanUpSessions();

	//////////////////////////////////////////////////////////////////////
	// Events

	/** Native Delegate when a local user has accepted an invite */
	/** 当本地用户接受邀请时的原生委托函数 */
	FCommonSessionOnUserRequestedSession OnUserRequestedSessionEvent;
	/** Event broadcast when a local user has accepted an invite */
	/** 当本地用户接受邀请时会触发此事件 */
	UPROPERTY(BlueprintAssignable, Category = "Events", meta = (DisplayName = "On User Requested Session"))
	FCommonSessionOnUserRequestedSession_Dynamic K2_OnUserRequestedSessionEvent;

	/** Native Delegate when a JoinSession call has completed */
	/** 当调用 JoinSession 操作完成后所使用的原生委托 */
	FCommonSessionOnJoinSessionComplete OnJoinSessionCompleteEvent;
	/** Event broadcast when a JoinSession call has completed */
	/** 当调用 JoinSession 操作完成后会触发此事件 */
	UPROPERTY(BlueprintAssignable, Category = "Events", meta = (DisplayName = "On Join Session Complete"))
	FCommonSessionOnJoinSessionComplete_Dynamic K2_OnJoinSessionCompleteEvent;

	/** Native Delegate when a CreateSession call has completed */
	/** 当创建会话的调用完成时的原生委托 */
	FCommonSessionOnCreateSessionComplete OnCreateSessionCompleteEvent;
	/** Event broadcast when a CreateSession call has completed */
	/** 当创建会话的调用完成时触发的事件 */
	UPROPERTY(BlueprintAssignable, Category = "Events", meta = (DisplayName = "On Create Session Complete"))
	FCommonSessionOnCreateSessionComplete_Dynamic K2_OnCreateSessionCompleteEvent;

	/** Native Delegate when the presentable session information has changed */
	/** 当可展示的会话信息发生更改时的原生委托函数 */
	FCommonSessionOnSessionInformationChanged OnSessionInformationChangedEvent;
	/** Event broadcast when the presentable session information has changed */
	/** 当可展示的会话信息发生更改时触发的事件 */
	UPROPERTY(BlueprintAssignable, Category = "Events", meta = (DisplayName = "On Session Information Changed"))
	FCommonSessionOnSessionInformationChanged_Dynamic K2_OnSessionInformationChangedEvent;

	/** Native Delegate when a platform session destroy has been requested */
	/** 当请求销毁平台会话时触发的事件 */
	FCommonSessionOnDestroySessionRequested OnDestroySessionRequestedEvent;
	/** Event broadcast when a platform session destroy has been requested */
	/** 当请求销毁平台会话时触发的事件 */
	UPROPERTY(BlueprintAssignable, Category = "Events", meta = (DisplayName = "On Leave Session Requested"))
	FCommonSessionOnDestroySessionRequested_Dynamic K2_OnDestroySessionRequestedEvent;

	/** Native Delegate for modifying the connect URL prior to a client travel */
	/** 用于在客户端移动前修改连接 URL 的原生委托函数 */
	FCommonSessionOnPreClientTravel OnPreClientTravelEvent;

	// Config settings, these can overridden in child classes or config files
	// 配置设置，这些设置可以在子类或配置文件中进行更改以进行覆盖。
	
	/** Sets the default value of bUseLobbies for session search and host requests */
	/** 设置用于会话搜索和主机请求的“bUseLobbies”属性的默认值 */
	UPROPERTY(Config)
	bool bUseLobbiesDefault = true;

	/** Sets the default value of bUseLobbiesVoiceChat for session host requests */
	/** 设置用于会话主持人请求的“bUseLobbiesVoiceChat”属性的默认值 */
	UPROPERTY(Config)
	bool bUseLobbiesVoiceChatDefault = false;

	/** Enables reservation beacon flow prior to server travel when creating or joining a game session */
	/** 在创建或加入游戏会话时，在服务器移动之前启用预订信标流 */
	UPROPERTY(Config)
	bool bUseBeacons = true;

protected:
	// Functions called during the process of creating or joining a session, these can be overidden for game-specific behavior
	// 在创建或加入会话的过程中所调用的函数，这些函数可针对特定游戏的需求进行重写以实现个性化功能。

	/** Called to fill in a session request from quick play host settings, can be overridden for game-specific behavior */
	/** 被调用以根据快速游戏主机设置填充会话请求，可针对特定游戏需求进行覆盖 */
	COMMONUSER_API virtual TSharedRef<FCommonOnlineSearchSettings> CreateQuickPlaySearchSettings(UCommonSession_HostSessionRequest* Request, UCommonSession_SearchSessionRequest* QuickPlayRequest);

	/** Called when a quick play search finishes, can be overridden for game-specific behavior */
	/** 当快速播放搜索完成时会调用此函数，可针对特定游戏需求进行自定义处理 */
	COMMONUSER_API virtual void HandleQuickPlaySearchFinished(bool bSucceeded, const FText& ErrorMessage, TWeakObjectPtr<APlayerController> JoiningOrHostingPlayer, TStrongObjectPtr<UCommonSession_HostSessionRequest> HostRequest);

	/** Called when traveling to a session fails */
	/** 当前往某个会话时出现错误时会调用此函数 */
	COMMONUSER_API virtual void TravelLocalSessionFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ReasonString);

	/** Called when a new session is either created or fails to be created */
	/** 当新会话被创建或创建失败时会调用此函数 */
	COMMONUSER_API virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/** Called to finalize session creation */
	/** 被调用以完成会话创建的最终步骤 */
	COMMONUSER_API virtual void FinishSessionCreation(bool bWasSuccessful);

	/** Called after traveling to the new hosted session map */
	/** 在前往新的托管会话地图后被调用 */
	COMMONUSER_API virtual void HandlePostLoadMap(UWorld* World);

protected:
	// Internal functions for initializing and handling results from the online systems

	COMMONUSER_API void BindOnlineDelegates();
	COMMONUSER_API void CreateOnlineSessionInternal(ULocalPlayer* LocalPlayer, UCommonSession_HostSessionRequest* Request);
	COMMONUSER_API void FindSessionsInternal(APlayerController* SearchingPlayer, const TSharedRef<FCommonOnlineSearchSettings>& InSearchSettings);
	COMMONUSER_API void JoinSessionInternal(ULocalPlayer* LocalPlayer, UCommonSession_SearchResult* Request);
	COMMONUSER_API void InternalTravelToSession(const FName SessionName);
	COMMONUSER_API void NotifyUserRequestedSession(const FPlatformUserId& PlatformUserId, UCommonSession_SearchResult* RequestedSession, const FOnlineResultInformation& RequestedSessionResult);
	COMMONUSER_API void NotifyJoinSessionComplete(const FOnlineResultInformation& Result);
	COMMONUSER_API void NotifyCreateSessionComplete(const FOnlineResultInformation& Result);
	COMMONUSER_API void NotifySessionInformationUpdated(ECommonSessionInformationState SessionStatusStr, const FString& GameMode = FString(), const FString& MapName = FString());
	COMMONUSER_API void NotifyDestroySessionRequested(const FPlatformUserId& PlatformUserId, const FName& SessionName);
	COMMONUSER_API void SetCreateSessionError(const FText& ErrorText);

#if COMMONUSER_OSSV1
	COMMONUSER_API void BindOnlineDelegatesOSSv1();
	COMMONUSER_API void CreateOnlineSessionInternalOSSv1(ULocalPlayer* LocalPlayer, UCommonSession_HostSessionRequest* Request);
	COMMONUSER_API void FindSessionsInternalOSSv1(ULocalPlayer* LocalPlayer);
	COMMONUSER_API void JoinSessionInternalOSSv1(ULocalPlayer* LocalPlayer, UCommonSession_SearchResult* Request);
	COMMONUSER_API TSharedRef<FCommonOnlineSearchSettings> CreateQuickPlaySearchSettingsOSSv1(UCommonSession_HostSessionRequest* Request, UCommonSession_SearchSessionRequest* QuickPlayRequest);
	COMMONUSER_API void CleanUpSessionsOSSv1();

	COMMONUSER_API void HandleSessionFailure(const FUniqueNetId& NetId, ESessionFailure::Type FailureType);
	COMMONUSER_API void HandleSessionUserInviteAccepted(const bool bWasSuccessful, const int32 LocalUserIndex, FUniqueNetIdPtr AcceptingUserId, const FOnlineSessionSearchResult& SearchResult);
	COMMONUSER_API void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
	COMMONUSER_API void OnRegisterLocalPlayerComplete_CreateSession(const FUniqueNetId& PlayerId, EOnJoinSessionCompleteResult::Type Result);
	COMMONUSER_API void OnUpdateSessionComplete(FName SessionName, bool bWasSuccessful);
	COMMONUSER_API void OnEndSessionComplete(FName SessionName, bool bWasSuccessful);
	COMMONUSER_API void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	COMMONUSER_API void OnDestroySessionRequested(int32 LocalUserNum, FName SessionName);
	COMMONUSER_API void OnFindSessionsComplete(bool bWasSuccessful);
	COMMONUSER_API void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	COMMONUSER_API void OnRegisterJoiningLocalPlayerComplete(const FUniqueNetId& PlayerId, EOnJoinSessionCompleteResult::Type Result);
	COMMONUSER_API void FinishJoinSession(EOnJoinSessionCompleteResult::Type Result);

#else
	COMMONUSER_API void BindOnlineDelegatesOSSv2();
	COMMONUSER_API void CreateOnlineSessionInternalOSSv2(ULocalPlayer* LocalPlayer, UCommonSession_HostSessionRequest* Request);
	COMMONUSER_API void FindSessionsInternalOSSv2(ULocalPlayer* LocalPlayer);
	COMMONUSER_API void JoinSessionInternalOSSv2(ULocalPlayer* LocalPlayer, UCommonSession_SearchResult* Request);
	COMMONUSER_API TSharedRef<FCommonOnlineSearchSettings> CreateQuickPlaySearchSettingsOSSv2(UCommonSession_HostSessionRequest* HostRequest, UCommonSession_SearchSessionRequest* SearchRequest);
	COMMONUSER_API void CleanUpSessionsOSSv2();

	/** Process a join request originating from the online service */
	COMMONUSER_API void OnLobbyJoinRequested(const UE::Online::FUILobbyJoinRequested& EventParams);

	/** Process a SESSION join request originating from the online service */
	COMMONUSER_API void OnSessionJoinRequested(const UE::Online::FUISessionJoinRequested& EventParams);

	/** Get the local user id for a given controller */
	COMMONUSER_API UE::Online::FAccountId GetAccountId(APlayerController* PlayerController) const;
	/** Get the lobby id for a given session name */
	COMMONUSER_API UE::Online::FLobbyId GetLobbyId(const FName SessionName) const;
	/** Event handle for UI lobby join requested */
	UE::Online::FOnlineEventDelegateHandle LobbyJoinRequestedHandle;

	/** Event handle for UI lobby session requested */
	UE::Online::FOnlineEventDelegateHandle SessionJoinRequestedHandle;

#endif // COMMONUSER_OSSV1

	COMMONUSER_API void CreateHostReservationBeacon();
	COMMONUSER_API void ConnectToHostReservationBeacon();
	COMMONUSER_API void DestroyHostReservationBeacon();

protected:
	/** The travel URL that will be used after session operations are complete */
	/** 在完成会话操作后将使用的旅行相关网址 */
	FString PendingTravelURL;

	/** Most recent result information for a session creation attempt, stored here to allow storing error codes for later */
	/** 本次会话创建尝试的最新结果信息，存储于此以便日后保存错误代码 */
	FOnlineResultInformation CreateSessionResult;

	/** True if we want to cancel the session after it is created */
	/** 如果我们希望在创建会话后取消该会话，则返回 true */
	bool bWantToDestroyPendingSession = false;

	/** True if this is a dedicated server, which doesn't require a LocalPlayer to create a session */
	/** 如果这是一个专用服务器，则返回 true，因为此类服务器无需由本地玩家来创建会话 */
	bool bIsDedicatedServer = false;

	/** Settings for the current search */
	/** 当前搜索的设置 */
	TSharedPtr<FCommonOnlineSearchSettings> SearchSettings;

	/** General beacon listener for registering beacons with */
	/** 用于注册信标的一般性信标监听器 */
	UPROPERTY(Transient)
	TWeakObjectPtr<AOnlineBeaconHost> BeaconHostListener;
	/** State of the beacon host */
	/** 蜂窝设备主机的状态 */
	UPROPERTY(Transient)
	TObjectPtr<UPartyBeaconState> ReservationBeaconHostState;
	/** Beacon controlling access to this game. */
	/** 用于控制对本游戏访问权限的信标。*/
	UPROPERTY(Transient)
	TWeakObjectPtr<APartyBeaconHost> ReservationBeaconHost;
	/** Common class object for beacon communication */
	/** 用于蜂窝通信的通用类对象 */
	UPROPERTY(Transient)
	TWeakObjectPtr<APartyBeaconClient> ReservationBeaconClient;

	/** Number of teams for beacon reservation */
	/** 用于信标预订的团队数量 */
	UPROPERTY(Config)
	int32 BeaconTeamCount = 2;
	/** Size of a team for beacon reservation */
	/** 用于信标预订的团队规模 */
	UPROPERTY(Config)
	int32 BeaconTeamSize = 8;
	/** Max number of beacon reservations */
	/** 最大信标预留数量 */
	UPROPERTY(Config)
	int32 BeaconMaxReservations = 16;
};
