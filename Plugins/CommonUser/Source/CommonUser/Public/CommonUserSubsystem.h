// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonUserTypes.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/OnlineReplStructs.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/WeakObjectPtr.h"
#include "GameplayTagContainer.h"
#include "CommonUserSubsystem.generated.h"

#if COMMONUSER_OSSV1
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineError.h"
#else
#include "Online/OnlineAsyncOpHandle.h"
#endif

class FNativeGameplayTag;
class IOnlineSubsystem;

/** List of tags used by the common user subsystem */
/** 常用用户子系统所使用的标签列表 */
struct FCommonUserTags
{
	// General severity levels and specific system messages
	// 基本的严重程度级别及具体的系统消息

	static COMMONUSER_API FNativeGameplayTag SystemMessage_Error;	// SystemMessage.Error
	static COMMONUSER_API FNativeGameplayTag SystemMessage_Warning; // SystemMessage.Warning
	static COMMONUSER_API FNativeGameplayTag SystemMessage_Display; // SystemMessage.Display

	/** All attempts to initialize a player failed, user has to do something before trying again */
	/** 所有尝试初始化玩家的操作均未成功，用户必须先采取一些行动，然后再重新尝试 */
	static COMMONUSER_API FNativeGameplayTag SystemMessage_Error_InitializeLocalPlayerFailed; // SystemMessage.Error.InitializeLocalPlayerFailed


	// Platform trait tags, it is expected that the game instance or other system calls SetTraitTags with these tags for the appropriate platform
	// 平台特有标签，建议游戏实例或其他系统使用这些标签调用 SetTraitTags 方法，以针对相应的平台进行设置。
	
	/** This tag means it is a console platform that directly maps controller IDs to different system users. If false, the same user can have multiple controllers */
	/** 此标签表示这是一个直接将控制器 ID 映射到不同系统用户的控制台平台。若为假值，则同一用户可以拥有多个控制器 */
	static COMMONUSER_API FNativeGameplayTag Platform_Trait_RequiresStrictControllerMapping; // Platform.Trait.RequiresStrictControllerMapping

	/** This tag means the platform has a single online user and all players use index 0 */
	/** 此标签表示该平台仅有一个在线用户，所有玩家均使用索引 0 */
	static COMMONUSER_API FNativeGameplayTag Platform_Trait_SingleOnlineUser; // Platform.Trait.SingleOnlineUser
};

/** Logical representation of an individual user, one of these will exist for all initialized local players */
/** 代表单个用户的逻辑结构，所有初始化后的本地玩家都将拥有这样的结构 */
UCLASS(MinimalAPI, BlueprintType)
class UCommonUserInfo : public UObject
{
	GENERATED_BODY()

public:
	/** Primary controller input device for this user, they could also have additional secondary devices */
	/** 本用户的主控制器输入设备，他们还可以拥有其他辅助设备 */
	UPROPERTY(BlueprintReadOnly, Category = UserInfo)
	FInputDeviceId PrimaryInputDevice;

	/** Specifies the logical user on the local platform, guest users will point to the primary user */
	/** 指定本地平台上的逻辑用户，访客用户将指向主用户 */
	UPROPERTY(BlueprintReadOnly, Category = UserInfo)
	FPlatformUserId PlatformUser;
	
	/** If this user is assigned a LocalPlayer, this will match the index in the GameInstance localplayers array once it is fully created */
	/** 如果此用户被分配了一个本地玩家角色，那么在游戏实例的本地玩家数组完全创建完成后，此条件将匹配该数组中的索引值 */
	UPROPERTY(BlueprintReadOnly, Category = UserInfo)
	int32 LocalPlayerIndex = -1;

	/** If true, this user is allowed to be a guest */
	/** 如果为真，则表示此用户可被视为访客 */
	UPROPERTY(BlueprintReadOnly, Category = UserInfo)
	bool bCanBeGuest = false;

	/** If true, this is a guest user attached to primary user 0 */
	/** 如果为真，则表示这是一个与主用户 0 相关联的访客用户 */
	UPROPERTY(BlueprintReadOnly, Category = UserInfo)
	bool bIsGuest = false;

	/** Overall state of the user's initialization process */
	/** 用户初始化过程的整体状态 */
	UPROPERTY(BlueprintReadOnly, Category = UserInfo)
	ECommonUserInitializationState InitializationState = ECommonUserInitializationState::Invalid;

	/** Returns true if this user has successfully logged in */
	/** 如果当前用户已成功登录，则返回 true */
	UFUNCTION(BlueprintCallable, Category = UserInfo)
	COMMONUSER_API bool IsLoggedIn() const;

	/** Returns true if this user is in the middle of logging in */
	/** 如果当前用户正在登录过程中，则返回 true */
	UFUNCTION(BlueprintCallable, Category = UserInfo)
	COMMONUSER_API bool IsDoingLogin() const;

	/** Returns the most recently queries result for a specific privilege, will return unknown if never queried */
	/** 返回针对特定权限的最近查询结果，若从未进行过查询则返回“未知” */
	UFUNCTION(BlueprintCallable, Category = UserInfo)
	COMMONUSER_API ECommonUserPrivilegeResult GetCachedPrivilegeResult(ECommonUserPrivilege Privilege, ECommonUserOnlineContext Context = ECommonUserOnlineContext::Game) const;

	/** Ask about the general availability of a feature, this combines cached results with state */
	/** 询问某一功能的总体可用性情况，此操作会将缓存结果与状态信息相结合 */
	UFUNCTION(BlueprintCallable, Category = UserInfo)
	COMMONUSER_API ECommonUserAvailability GetPrivilegeAvailability(ECommonUserPrivilege Privilege) const;

	/** Returns the net id for the given context */
	/** 返回给定上下文的净标识符 */
	UFUNCTION(BlueprintCallable, Category = UserInfo)
	COMMONUSER_API FUniqueNetIdRepl GetNetId(ECommonUserOnlineContext Context = ECommonUserOnlineContext::Game) const;

	/** Returns the user's human readable nickname, this will return the value that was cached during UpdateCachedNetId or SetNickname */
	/** 返回用户的可读人性别化昵称，此值将为在“更新缓存网络标识”或“设置昵称”操作中缓存的值 */
	UFUNCTION(BlueprintCallable, Category = UserInfo)
	COMMONUSER_API FString GetNickname(ECommonUserOnlineContext Context = ECommonUserOnlineContext::Game) const;

	/** Modify the user's human readable nickname, this can be used when setting up multiple guests but will get overwritten with the platform nickname for real users */
	/** 修改用户的可读性人名，此功能可用于为多个访客设置昵称，但实际用户使用的平台昵称会覆盖此设置 */
	UFUNCTION(BlueprintCallable, Category = UserInfo)
	COMMONUSER_API void SetNickname(const FString& NewNickname, ECommonUserOnlineContext Context = ECommonUserOnlineContext::Game);

	/** Returns an internal debug string for this player */
	/** 返回此玩家的内部调试字符串 */
	UFUNCTION(BlueprintCallable, Category = UserInfo)
	COMMONUSER_API FString GetDebugString() const;

	/** Accessor for platform user id */
	/** 平台用户 ID 访问器 */
	COMMONUSER_API FPlatformUserId GetPlatformUserId() const;

	/** Gets the platform user index for older functions expecting an integer */
	/** 获取用于较旧函数的平台用户索引（这些函数期望输入的是一个整数） */
	COMMONUSER_API int32 GetPlatformUserIndex() const;

	// Internal data, only intended to be accessed by online subsystems
	// 内部数据，仅供在线子系统访问使用

	/** Cached data for each online system */
	/** 每个在线系统的缓存数据 */
	struct FCachedData
	{
		/** Cached net id per system */
		/** 每个系统的缓存网络标识 */
		FUniqueNetIdRepl CachedNetId;

		/** Cached nickanem, updated whenever net ID might change */
		/** 缓存的用户名，每当网络标识可能发生变更时会进行更新 */
		FString CachedNickname;

		/** Cached values of various user privileges */
		/** 不同用户权限的缓存值 */
		TMap<ECommonUserPrivilege, ECommonUserPrivilegeResult> CachedPrivileges;
	};

	/** Per context cache, game will always exist but others may not */
	/** 按照不同的环境设置，游戏始终会存在，但其他内容可能不会 */
	TMap<ECommonUserOnlineContext, FCachedData> CachedDataMap;
	
	/** Looks up cached data using resolution rules */
	/** 根据解析规则查找缓存数据 */
	COMMONUSER_API FCachedData* GetCachedData(ECommonUserOnlineContext Context);
	COMMONUSER_API const FCachedData* GetCachedData(ECommonUserOnlineContext Context) const;

	/** Updates cached privilege results, will propagate to game if needed */
	/** 更新缓存的权限结果，如有需要将同步至游戏端 */
	COMMONUSER_API void UpdateCachedPrivilegeResult(ECommonUserPrivilege Privilege, ECommonUserPrivilegeResult Result, ECommonUserOnlineContext Context);

	/** Updates cached privilege results, will propagate to game if needed */
	/** 更新缓存的权限结果，如有需要将同步至游戏端 */
	COMMONUSER_API void UpdateCachedNetId(const FUniqueNetIdRepl& NewId, ECommonUserOnlineContext Context);

	/** Return the subsystem this is owned by */
	/** 返回此系统所归属的子系统 */
	COMMONUSER_API class UCommonUserSubsystem* GetSubsystem() const;
};


/** Delegates when initialization processes succeed or fail */
/** 用于记录初始化过程成功或失败的情况 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FCommonUserOnInitializeCompleteMulticast, const UCommonUserInfo*, UserInfo, bool, bSuccess, FText, Error, ECommonUserPrivilege, RequestedPrivilege, ECommonUserOnlineContext, OnlineContext);
DECLARE_DYNAMIC_DELEGATE_FiveParams(FCommonUserOnInitializeComplete, const UCommonUserInfo*, UserInfo, bool, bSuccess, FText, Error, ECommonUserPrivilege, RequestedPrivilege, ECommonUserOnlineContext, OnlineContext);

/** Delegate when a system error message is sent, the game can choose to display it to the user using the type tag */
/** 当系统错误消息被发送时，该方法将被调用。游戏可以根据类型标签选择向用户显示该错误消息 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCommonUserHandleSystemMessageDelegate, FGameplayTag, MessageType, FText, TitleText, FText, BodyText);

/** Delegate when a privilege changes, this can be bound to see if online status/etc changes during gameplay */
/** 当权限发生变更时会触发此回调函数，此回调函数可用于检测游戏过程中在线状态等的变化情况 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FCommonUserAvailabilityChangedDelegate, const UCommonUserInfo*, UserInfo, ECommonUserPrivilege, Privilege, ECommonUserAvailability, OldAvailability, ECommonUserAvailability, NewAvailability);


/** Parameter struct for initialize functions, this would normally be filled in by wrapper functions like async nodes */
/** 用于初始化函数的参数结构体，通常由诸如异步节点之类的封装函数来填充 */
USTRUCT(BlueprintType)
struct FCommonUserInitializeParams
{
	GENERATED_BODY()
	
	/** What local player index to use, can specify one above current if can create player is enabled */
	/** 指定使用的本地玩家索引，如果“创建玩家”功能已启用，则可以指定高于当前索引的值 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Default)
	int32 LocalPlayerIndex = 0;

	/** Deprecated method of selecting platform user and input device */
	/** 已弃用的用于选择平台用户和输入设备的方法 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Default)
	int32 ControllerId = -1;

	/** Primary controller input device for this user, they could also have additional secondary devices */
	/** 本用户的主控制器输入设备，他们还可以拥有其他辅助设备 */
	UPROPERTY(BlueprintReadOnly, Category = UserInfo)
	FInputDeviceId PrimaryInputDevice;

	/** Specifies the logical user on the local platform */
	/** 指定本地平台上的逻辑用户 */
	UPROPERTY(BlueprintReadOnly, Category = UserInfo)
	FPlatformUserId PlatformUser;
	
	/** Generally either CanPlay or CanPlayOnline, specifies what level of privilege is required */
	/** 通常为“CanPlay”或“CanPlayOnline”，用于指定所需的权限级别 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Default)
	ECommonUserPrivilege RequestedPrivilege = ECommonUserPrivilege::CanPlay;

	/** What specific online context to log in to, game means to login to all relevant ones */
	/** 指定要登录的具体在线环境，游戏模式意味着登录所有相关环境 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Default)
	ECommonUserOnlineContext OnlineContext = ECommonUserOnlineContext::Game;

	/** True if this is allowed to create a new local player for initial login */
	/** 如果允许在首次登录时创建新的本地玩家，则返回 true */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Default)
	bool bCanCreateNewLocalPlayer = false;

	/** True if this player can be a guest user without an actual online presence */
	/** 如果此玩家可以作为访客用户存在，而无需具备实际的在线活动记录，则返回 true */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Default)
	bool bCanUseGuestLogin = false;

	/** True if we should not show login errors, the game will be responsible for displaying them */
	/** 如果我们不应显示登录错误信息，则应设为“真”，此时游戏将负责显示这些错误信息 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Default)
	bool bSuppressLoginErrors = false;

	/** If bound, call this dynamic delegate at completion of login */
	/** 如果已绑定，则在登录完成时调用此动态委托 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Default)
	FCommonUserOnInitializeComplete OnUserInitializeComplete;
};

/**
 * Game subsystem that handles queries and changes to user identity and login status.
 * One subsystem is created for each game instance and can be accessed from blueprints or C++ code.
 * If a game-specific subclass exists, this base subsystem will not be created.
 *
 * 该子系统负责处理用户身份和登录状态的查询及更改操作。
 * 每个游戏实例都会创建一个独立的子系统，并且可以从蓝图或 C++ 代码中访问该子系统。
 * 如果存在特定于游戏的子类，则不会创建这个基础子系统。
 * 
 */
UCLASS(MinimalAPI, BlueprintType, Config=Engine)
class UCommonUserSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UCommonUserSubsystem() { }

	COMMONUSER_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	COMMONUSER_API virtual void Deinitialize() override;
	COMMONUSER_API virtual bool ShouldCreateSubsystem(UObject* Outer) const override;


	/** BP delegate called when any requested initialization request completes */
	/** 当任何请求的初始化操作完成时会调用此 BP（业务处理）委托函数 */
	UPROPERTY(BlueprintAssignable, Category = CommonUser)
	FCommonUserOnInitializeCompleteMulticast OnUserInitializeComplete;

	/** BP delegate called when the system sends an error/warning message */
	/** 当系统发送错误/警告消息时调用的 BP（业务处理）委托函数 */
	UPROPERTY(BlueprintAssignable, Category = CommonUser)
	FCommonUserHandleSystemMessageDelegate OnHandleSystemMessage;

	/** BP delegate called when privilege availability changes for a user  */
	/** 当用户的权限可用性发生变化时调用的 BP（业务流程）委托函数 */
	UPROPERTY(BlueprintAssignable, Category = CommonUser)
	FCommonUserAvailabilityChangedDelegate OnUserPrivilegeChanged;

	/** Send a system message via OnHandleSystemMessage */
	/** 通过“OnHandleSystemMessage”方法发送系统消息 */
	UFUNCTION(BlueprintCallable, Category = CommonUser)
	COMMONUSER_API virtual void SendSystemMessage(FGameplayTag MessageType, FText TitleText, FText BodyText);

	/** Sets the maximum number of local players, will not destroy existing ones */
	/** 设置本地玩家的最大数量（不会删除已存在的玩家） */
	UFUNCTION(BlueprintCallable, Category = CommonUser)
	COMMONUSER_API virtual void SetMaxLocalPlayers(int32 InMaxLocalPLayers);

	/** Gets the maximum number of local players */
	/** 获取本地玩家的最大数量 */
	UFUNCTION(BlueprintPure, Category = CommonUser)
	COMMONUSER_API int32 GetMaxLocalPlayers() const;

	/** Gets the current number of local players, will always be at least 1 */
	/** 获取当前本地玩家的数量，该数量始终至少为 1 */
	UFUNCTION(BlueprintPure, Category = CommonUser)
	COMMONUSER_API int32 GetNumLocalPlayers() const;

	/** Returns the state of initializing the specified local player */
	/** 返回指定本地玩家初始化状态 */
	UFUNCTION(BlueprintPure, Category = CommonUser)
	COMMONUSER_API ECommonUserInitializationState GetLocalPlayerInitializationState(int32 LocalPlayerIndex) const;

	/** Returns the user info for a given local player index in game instance, 0 is always valid in a running game */
	/** 返回游戏实例中指定本地玩家索引对应的用户信息，运行中的游戏中的 0 索引始终有效 */
	UFUNCTION(BlueprintCallable, BlueprintPure = False, Category = CommonUser)
	COMMONUSER_API const UCommonUserInfo* GetUserInfoForLocalPlayerIndex(int32 LocalPlayerIndex) const;

	/** Deprecated, use PlatformUserId when available */
	/** 已弃用，如可用时请使用 PlatformUserId */
	UFUNCTION(BlueprintCallable, BlueprintPure = False, Category = CommonUser)
	COMMONUSER_API const UCommonUserInfo* GetUserInfoForPlatformUserIndex(int32 PlatformUserIndex) const;

	/** Returns the primary user info for a given platform user index. Can return null */
	/** 返回给定平台用户索引对应的主用户信息。可能返回 null  */
	UFUNCTION(BlueprintCallable, BlueprintPure = False, Category = CommonUser)
	COMMONUSER_API const UCommonUserInfo* GetUserInfoForPlatformUser(FPlatformUserId PlatformUser) const;

	/** Returns the user info for a unique net id. Can return null */
	/** 返回特定网络账号对应的用户信息。可能返回 null  */
	UFUNCTION(BlueprintCallable, BlueprintPure = False, Category = CommonUser)
	COMMONUSER_API const UCommonUserInfo* GetUserInfoForUniqueNetId(const FUniqueNetIdRepl& NetId) const;

	/** Deprecated, use InputDeviceId when available */
	/** 已弃用，如可用时请使用 InputDeviceId */
	UFUNCTION(BlueprintCallable, BlueprintPure = False, Category = CommonUser)
	COMMONUSER_API const UCommonUserInfo* GetUserInfoForControllerId(int32 ControllerId) const;

	/** Returns the user info for a given input device. Can return null */
	/** 返回给定输入设备的用户信息。可能返回 null  */
	UFUNCTION(BlueprintCallable, BlueprintPure = False, Category = CommonUser)
	COMMONUSER_API const UCommonUserInfo* GetUserInfoForInputDevice(FInputDeviceId InputDevice) const;

	/**
	 * Tries to start the process of creating or updating a local player, including logging in and creating a player controller.
	 * When the process has succeeded or failed, it will broadcast the OnUserInitializeComplete delegate.
	 *
	 * @param LocalPlayerIndex	Desired index of LocalPlayer in Game Instance, 0 will be primary player and 1+ for local multiplayer
	 * @param PrimaryInputDevice The physical controller that should be mapped to this user, will use the default device if invalid
	 * @param bCanUseGuestLogin	If true, this player can be a guest without a real Unique Net Id
	 *
	 * @returns true if the process was started, false if it failed before properly starting
	 */
	/**
	 * 尝试启动创建或更新本地玩家的流程，包括登录操作和创建玩家控制器。
	 * 当该流程成功或失败时，会广播“OnUserInitializeComplete”委托。
	 * @参数 LocalPlayerIndex：游戏实例中本地玩家的期望索引值，0 表示为主玩家，1 及以上表示本地多人游戏模式
	 * @参数 PrimaryInputDevice：应与该用户关联的物理控制器，无效时将使用默认设备
	 * @参数 bCanUseGuestLogin：如果为真，则此玩家可以作为访客存在，无需拥有真实的唯一网络标识符
	 * @返回值：如果进程已启动则返回 true，否则如果在正确启动之前就失败则返回 false
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category = CommonUser)
	COMMONUSER_API virtual bool TryToInitializeForLocalPlay(int32 LocalPlayerIndex, FInputDeviceId PrimaryInputDevice, bool bCanUseGuestLogin);

	/**
	 * Starts the process of taking a locally logged in user and doing a full online login including account permission checks.
	 * When the process has succeeded or failed, it will broadcast the OnUserInitializeComplete delegate.
	 *
	 * @param LocalPlayerIndex	Index of existing LocalPlayer in Game Instance
	 *
	 * @returns true if the process was started, false if it failed before properly starting
	 */
	/**
	 * 开始对本地登录的用户进行完整在线登录的操作，包括账户权限检查。
	 * 当该过程成功或失败时，会广播“OnUserInitializeComplete”委托。
	 * @参数 LocalPlayerIndex：游戏实例中现有本地玩家的索引号*
	 * @返回值：如果进程已启动则返回 true，若在正确启动之前失败则返回 false
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category = CommonUser)
	COMMONUSER_API virtual bool TryToLoginForOnlinePlay(int32 LocalPlayerIndex);

	/**
	 * Starts a general user login and initialization process, using the params structure to determine what to log in to.
	 * When the process has succeeded or failed, it will broadcast the OnUserInitializeComplete delegate.
	 * AsyncAction_CommonUserInitialize provides several wrapper functions for using this in an Event graph.
	 *
	 * @returns true if the process was started, false if it failed before properly starting
	 */
	/**
	 * 启动通用用户登录及初始化流程，通过参数结构来确定要登录的账号。
	 * 当该流程成功或失败时，会广播“OnUserInitializeComplete”委托。
	 * AsyncAction_CommonUserInitialize 提供了几个用于在事件图中使用此功能的封装函数。
	 * @返回值：如果进程已启动则返回 true，若在正确启动之前失败则返回 false
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category = CommonUser)
	COMMONUSER_API virtual bool TryToInitializeUser(FCommonUserInitializeParams Params);

	/** 
	 * Starts the process of listening for user input for new and existing controllers and logging them.
	 * This will insert a key input handler on the active GameViewportClient and is turned off by calling again with empty key arrays.
	 *
	 * @param AnyUserKeys		Listen for these keys for any user, even the default user. Set this for an initial press start screen or empty to disable
	 * @param NewUserKeys		Listen for these keys for a new user without a player controller. Set this for splitscreen/local multiplayer or empty to disable
	 * @param Params			Params passed to TryToInitializeUser after detecting key input
	 */
	/**
	 * 开始监听用户对新控制器和现有控制器的操作输入，并将这些输入记录下来。
	 * 这会将一个按键输入处理程序插入到活动的游戏视图客户端中，并可通过再次传入空的按键数组来关闭该处理程序。
	 * @参数 AnyUserKeys：监听任何用户的这些按键，包括默认用户。将此设置为初始启动屏幕的参数值或留空可禁用该功能。
	 * @参数 NewUserKeys：监听新用户的这些按键（该用户没有玩家控制器）。将此设置为分屏/本地多人游戏的参数值或留空可禁用该功能。
	 * @参数 Params：在检测到按键输入后传递给 TryToInitializeUser 的参数值。
	 * 
	 */	
	UFUNCTION(BlueprintCallable, Category = CommonUser)
	COMMONUSER_API virtual void ListenForLoginKeyInput(TArray<FKey> AnyUserKeys, TArray<FKey> NewUserKeys, FCommonUserInitializeParams Params);

	/** Attempts to cancel an in-progress initialization attempt, this may not work on all platforms but will disable callbacks */
	/** 尝试取消正在进行的初始化操作，此操作可能并非在所有平台上都能生效，但会禁用回调函数 */
	UFUNCTION(BlueprintCallable, Category = CommonUser)
	COMMONUSER_API virtual bool CancelUserInitialization(int32 LocalPlayerIndex);

	/** Logs a player out of any online systems, and optionally destroys the player entirely if it's not the first one */
	/** 会将玩家从任何在线系统中退出，并且如果这不是首次操作，还会选择性地彻底删除该玩家的账号 */
	UFUNCTION(BlueprintCallable, Category = CommonUser)
	COMMONUSER_API virtual bool TryToLogOutUser(int32 LocalPlayerIndex, bool bDestroyPlayer = false);

	/** Resets the login and initialization state when returning to the main menu after an error */
	/** 在出现错误后返回主菜单时，重置登录和初始化状态 */
	UFUNCTION(BlueprintCallable, Category = CommonUser)
	COMMONUSER_API virtual void ResetUserState();

	/** Returns true if this this could be a real platform user with a valid identity (even if not currently logged in)  */
	/** 如果此对象代表的是拥有有效身份的真正平台用户（即使当前未登录）则返回 true  */
	COMMONUSER_API virtual bool IsRealPlatformUserIndex(int32 PlatformUserIndex) const;

	/** Returns true if this this could be a real platform user with a valid identity (even if not currently logged in) */
	/** 如果此对象可能是拥有有效身份的真正平台用户（即便当前未登录）则返回 true */
	COMMONUSER_API virtual bool IsRealPlatformUser(FPlatformUserId PlatformUser) const;

	/** Converts index to id */
	/** 将索引转换为 ID */
	COMMONUSER_API virtual FPlatformUserId GetPlatformUserIdForIndex(int32 PlatformUserIndex) const;

	/** Converts id to index */
	/** 将“id”转换为“索引” */
	COMMONUSER_API virtual int32 GetPlatformUserIndexForId(FPlatformUserId PlatformUser) const;

	/** Gets the user for an input device */
	/** 获取输入设备的用户信息 */
	COMMONUSER_API virtual FPlatformUserId GetPlatformUserIdForInputDevice(FInputDeviceId InputDevice) const;

	/** Gets a user's primary input device id */
	/** 获取用户的主输入设备 ID */
	COMMONUSER_API virtual FInputDeviceId GetPrimaryInputDeviceForPlatformUser(FPlatformUserId PlatformUser) const;

	/** Call from game code to set the cached trait tags when platform state or options changes */
	/** 从游戏代码中调用，用于在平台状态或选项发生变化时设置缓存的特征标签 */
	COMMONUSER_API virtual void SetTraitTags(const FGameplayTagContainer& InTags);

	/** Gets the current tags that affect feature avialability */
	/** 获取当前影响功能可用性的标签 */
	const FGameplayTagContainer& GetTraitTags() const { return CachedTraitTags; }

	/** Checks if a specific platform/feature tag is enabled */
	/** 检查特定的平台/功能标签是否已启用 */
	UFUNCTION(BlueprintPure, Category=CommonUser)
	bool HasTraitTag(const FGameplayTag TraitTag) const { return CachedTraitTags.HasTag(TraitTag); }

	/** Checks to see if we should display a press start/input confirmation screen at startup. Games can call this or check the trait tags directly */
	/** 检查是否应在启动时显示“开始游戏/输入确认”界面。游戏可以调用此方法，也可以直接检查特性标签来确定是否显示该界面 */
	UFUNCTION(BlueprintPure, BlueprintPure, Category=CommonUser)
	COMMONUSER_API virtual bool ShouldWaitForStartInput() const;


	// Functions for accessing low-level online system information
	// 用于访问底层在线系统信息的函数
	
#if COMMONUSER_OSSV1
	/** Returns OSS interface of specific type, will return null if there is no type */
	/** 返回特定类型的 OSS 接口，若不存在该类型则返回 null */
	COMMONUSER_API IOnlineSubsystem* GetOnlineSubsystem(ECommonUserOnlineContext Context = ECommonUserOnlineContext::Game) const;

	/** Returns identity interface of specific type, will return null if there is no type */
	/** 返回特定类型的标识接口，若不存在该类型则返回 null */
	COMMONUSER_API IOnlineIdentity* GetOnlineIdentity(ECommonUserOnlineContext Context = ECommonUserOnlineContext::Game) const;

	/** Returns human readable name of OSS system */
	/** 返回 OSS 系统的可读名称 */
	COMMONUSER_API FName GetOnlineSubsystemName(ECommonUserOnlineContext Context = ECommonUserOnlineContext::Game) const;

	/** Returns the current online connection status */
	/** 返回当前的在线连接状态 */
	COMMONUSER_API EOnlineServerConnectionStatus::Type GetConnectionStatus(ECommonUserOnlineContext Context = ECommonUserOnlineContext::Game) const;
#else
	/** Get the services provider type, or None if there isn't one. */
	/** 获取服务提供商类型，若无则返回 None 。*/
	COMMONUSER_API UE::Online::EOnlineServices GetOnlineServicesProvider(ECommonUserOnlineContext Context = ECommonUserOnlineContext::Game) const;
	
	/** Returns auth interface of specific type, will return null if there is no type */
	/** 返回特定类型的认证接口，若不存在该类型则返回 null */
	COMMONUSER_API UE::Online::IAuthPtr GetOnlineAuth(ECommonUserOnlineContext Context = ECommonUserOnlineContext::Game) const;

	/** Returns the current online connection status */
	/** 返回当前的在线连接状态 */
	COMMONUSER_API UE::Online::EOnlineServicesConnectionStatus GetConnectionStatus(ECommonUserOnlineContext Context = ECommonUserOnlineContext::Game) const;
#endif

	/** Returns true if we are currently connected to backend servers */
	/** 返回值为真表示我们当前已与后端服务器建立连接 */
	COMMONUSER_API bool HasOnlineConnection(ECommonUserOnlineContext Context = ECommonUserOnlineContext::Game) const;

	/** Returns the current login status for a player on the specified online system, only works for real platform users */
	/** 返回指定在线系统中玩家的当前登录状态，仅适用于真实平台上的用户 */
	COMMONUSER_API ELoginStatusType GetLocalUserLoginStatus(FPlatformUserId PlatformUser, ECommonUserOnlineContext Context = ECommonUserOnlineContext::Game) const;

	/** Returns the unique net id for a local platform user */
	/** 返回本地平台用户唯一的网络标识符 */
	COMMONUSER_API FUniqueNetIdRepl GetLocalUserNetId(FPlatformUserId PlatformUser, ECommonUserOnlineContext Context = ECommonUserOnlineContext::Game) const;

	/** Returns the nickname for a local platform user, this is cached in common user Info */
	/** 返回本地平台用户的昵称，该昵称已缓存在通用用户信息中 */
	COMMONUSER_API FString GetLocalUserNickname(FPlatformUserId PlatformUser, ECommonUserOnlineContext Context = ECommonUserOnlineContext::Game) const;

	/** Convert a user id to a debug string */
	/** 将用户 ID 转换为调试字符串 */
	COMMONUSER_API FString PlatformUserIdToString(FPlatformUserId UserId);

	/** Convert a context to a debug string */
	/** 将上下文转换为调试字符串 */
	COMMONUSER_API FString ECommonUserOnlineContextToString(ECommonUserOnlineContext Context);

	/** Returns human readable string for privilege checks */
	/** 返回用于权限检查的可读性较强的字符串 */
	COMMONUSER_API virtual FText GetPrivilegeDescription(ECommonUserPrivilege Privilege) const;
	COMMONUSER_API virtual FText GetPrivilegeResultDescription(ECommonUserPrivilegeResult Result) const;

	/** 
	 * Starts the process of login for an existing local user, will return false if callback was not scheduled 
	 * This activates the low level state machine and does not modify the initialization state on user info
	 */
	/**
	 * 开始为现有本地用户进行登录操作，若未安排回调则会返回 false
	 * 这会激活底层状态机，并且不会修改用户信息的初始化状态
	 * 
	 */
	DECLARE_DELEGATE_FiveParams(FOnLocalUserLoginCompleteDelegate, const UCommonUserInfo* /*UserInfo*/, ELoginStatusType /*NewStatus*/, FUniqueNetIdRepl /*NetId*/, const TOptional<FOnlineErrorType>& /*Error*/, ECommonUserOnlineContext /*Type*/);
	COMMONUSER_API virtual bool LoginLocalUser(const UCommonUserInfo* UserInfo, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext Context, FOnLocalUserLoginCompleteDelegate OnComplete);

	/** Assign a local player to a specific local user and call callbacks as needed */
	/** 将本地玩家分配给特定的本地用户，并根据需要调用回调函数 */
	COMMONUSER_API virtual void SetLocalPlayerUserInfo(ULocalPlayer* LocalPlayer, const UCommonUserInfo* UserInfo);

	/** Resolves a context that has default behavior into a specific context */
	/** 将具有默认行为的上下文转换为特定的上下文 */
	COMMONUSER_API ECommonUserOnlineContext ResolveOnlineContext(ECommonUserOnlineContext Context) const;

	/** True if there is a separate platform and service interface */
	/** 如果存在独立的平台及服务接口，则为真 */
	COMMONUSER_API bool HasSeparatePlatformContext() const;

protected:
	/** Internal structure that caches status and pointers for each online context */
	/** 用于缓存每个在线会话的状态和指针的内部结构 */
	struct FOnlineContextCache
	{
#if COMMONUSER_OSSV1
		/** Pointer to base subsystem, will stay valid as long as game instance does */
		/** 指向基础子系统的指针，只要游戏实例存在，该指针就会一直有效 */
		IOnlineSubsystem* OnlineSubsystem = nullptr;

		/** Cached identity system, this will always be valid */
		/** 缓存的身份系统，此系统始终有效 */
		IOnlineIdentityPtr IdentityInterface;

		/** Last connection status that was passed into the HandleNetworkConnectionStatusChanged hander */
		/** 传入“处理网络连接状态变化”处理程序的最后连接状态 */
		EOnlineServerConnectionStatus::Type	CurrentConnectionStatus = EOnlineServerConnectionStatus::Normal;
#else
		/** Online services, accessor to specific services */
		/** 在线服务，特定服务的访问器 */
		UE::Online::IOnlineServicesPtr OnlineServices;
		/** Cached auth service */
		/** 缓存认证服务 */
		UE::Online::IAuthPtr AuthService;
		/** Login status changed event handle */
		/** 登录状态变化事件处理程序 */
		UE::Online::FOnlineEventDelegateHandle LoginStatusChangedHandle;
		/** Connection status changed event handle */
		/** 连接状态变化事件处理程序 */
		UE::Online::FOnlineEventDelegateHandle ConnectionStatusChangedHandle;
		/** Last connection status that was passed into the HandleNetworkConnectionStatusChanged hander */
		/** 传入“处理网络连接状态变化”处理程序的最后连接状态 */
		UE::Online::EOnlineServicesConnectionStatus CurrentConnectionStatus = UE::Online::EOnlineServicesConnectionStatus::NotConnected;
#endif

		/** Resets state, important to clear all shared ptrs */
		/** 重置状态，此操作对于清除所有共享指针至关重要 */
		void Reset()
		{
#if COMMONUSER_OSSV1
			OnlineSubsystem = nullptr;
			IdentityInterface.Reset();
			CurrentConnectionStatus = EOnlineServerConnectionStatus::Normal;
#else
			OnlineServices.Reset();
			AuthService.Reset();
			CurrentConnectionStatus = UE::Online::EOnlineServicesConnectionStatus::NotConnected;
#endif
		}
	};

	/** Internal structure to represent an in-progress login request */
	/** 用于表示正在进行的登录请求的内部结构 */
	struct FUserLoginRequest : public TSharedFromThis<FUserLoginRequest>
	{
		FUserLoginRequest(UCommonUserInfo* InUserInfo, ECommonUserPrivilege InPrivilege, ECommonUserOnlineContext InContext, FOnLocalUserLoginCompleteDelegate&& InDelegate)
			: UserInfo(TWeakObjectPtr<UCommonUserInfo>(InUserInfo))
			, DesiredPrivilege(InPrivilege)
			, DesiredContext(InContext)
			, Delegate(MoveTemp(InDelegate))
			{}

		/** Which local user is trying to log on */
		/** 正在尝试登录的本地用户是谁 */
		TWeakObjectPtr<UCommonUserInfo> UserInfo;

		/** Overall state of login request, could come from many sources */
		/** 登录请求的整体状态，可能来自多个来源 */
		ECommonUserAsyncTaskState OverallLoginState = ECommonUserAsyncTaskState::NotStarted;

		/** State of attempt to use platform auth. When started, this immediately transitions to Failed for OSSv1, as we do not support platform auth there. */
		/** 表示尝试使用平台认证的状态。当启动该操作时，对于 OSSv1 来说，此状态会立即变为“失败”，因为我们在该版本中不支持平台认证。*/
		ECommonUserAsyncTaskState TransferPlatformAuthState = ECommonUserAsyncTaskState::NotStarted;

		/** State of attempt to use AutoLogin */
		/** 使用自动登录的尝试状态 */
		ECommonUserAsyncTaskState AutoLoginState = ECommonUserAsyncTaskState::NotStarted;

		/** State of attempt to use external login UI */
		/** 使用外部登录界面的尝试状态 */
		ECommonUserAsyncTaskState LoginUIState = ECommonUserAsyncTaskState::NotStarted;

		/** Final privilege to that is requested */
		/** 最终所要求的权限 */
		ECommonUserPrivilege DesiredPrivilege = ECommonUserPrivilege::Invalid_Count;

		/** State of attempt to request the relevant privilege */
		/** 请求相关权限的尝试状态 */
		ECommonUserAsyncTaskState PrivilegeCheckState = ECommonUserAsyncTaskState::NotStarted;

		/** The final context to log into */
		/** 最终需要记录的上下文信息 */
		ECommonUserOnlineContext DesiredContext = ECommonUserOnlineContext::Invalid;

		/** What online system we are currently logging into */
		/** 我们当前登录的是哪个在线系统 */
		ECommonUserOnlineContext CurrentContext = ECommonUserOnlineContext::Invalid;

		/** User callback for completion */
		/** 用户完成操作的回调函数 */
		FOnLocalUserLoginCompleteDelegate Delegate;

		/** Most recent/relevant error to display to user */
		/** 显示给用户的最新/相关错误信息 */
		TOptional<FOnlineErrorType> Error;
	};


	/** Create a new user info object */
	/** 创建一个新的用户信息对象 */
	COMMONUSER_API virtual UCommonUserInfo* CreateLocalUserInfo(int32 LocalPlayerIndex);

	/** Deconst wrapper for const getters */
	/** 对常量获取器的解常量包装器 */
	FORCEINLINE UCommonUserInfo* ModifyInfo(const UCommonUserInfo* Info) { return const_cast<UCommonUserInfo*>(Info); }

	/** Refresh user info from OSS */
	/** 从 OSS 中刷新用户信息 */
	COMMONUSER_API virtual void RefreshLocalUserInfo(UCommonUserInfo* UserInfo);

	/** Possibly send privilege availability notification, compares current value to cached old value */
	/** 可能会发送权限可用性通知，将当前值与缓存的旧值进行比较 */
	COMMONUSER_API virtual void HandleChangedAvailability(UCommonUserInfo* UserInfo, ECommonUserPrivilege Privilege, ECommonUserAvailability OldAvailability);

	/** Updates the cached privilege on a user and notifies delegate */
	/** 更新用户的缓存权限并通知委托方 */
	COMMONUSER_API virtual void UpdateUserPrivilegeResult(UCommonUserInfo* UserInfo, ECommonUserPrivilege Privilege, ECommonUserPrivilegeResult Result, ECommonUserOnlineContext Context);

	/** Gets internal data for a type of online system, can return null for service */
	/** 获取某种在线系统的内部数据，对于某些服务可能会返回 null  */
	COMMONUSER_API const FOnlineContextCache* GetContextCache(ECommonUserOnlineContext Context = ECommonUserOnlineContext::Game) const;
	COMMONUSER_API FOnlineContextCache* GetContextCache(ECommonUserOnlineContext Context = ECommonUserOnlineContext::Game);

	/** Create and set up system objects before delegates are bound */
	/** 在委托绑定之前创建并设置系统对象 */
	COMMONUSER_API virtual void CreateOnlineContexts();
	COMMONUSER_API virtual void DestroyOnlineContexts();

	/** Bind online delegates */
	/** 绑定在线委托 */
	COMMONUSER_API virtual void BindOnlineDelegates();

	/** Forcibly logs out and deinitializes a single user */
	/** 强制退出并解除单个用户的登录状态 */
	COMMONUSER_API virtual void LogOutLocalUser(FPlatformUserId PlatformUser);

	/** Performs the next step of a login request, which could include completing it. Returns true if it's done */
	/** 执行登录请求的下一步操作，这可能包括完成整个操作。如果已完成则返回 true */
	COMMONUSER_API virtual void ProcessLoginRequest(TSharedRef<FUserLoginRequest> Request);

	/** Call login on OSS, with platform auth from the platform OSS. Return true if AutoLogin started */
	/** 调用 OSS 中的登录功能，使用来自平台 OSS 的平台认证信息。如果自动登录已启动，则返回 true */
	COMMONUSER_API virtual bool TransferPlatformAuth(FOnlineContextCache* System, TSharedRef<FUserLoginRequest> Request, FPlatformUserId PlatformUser);

	/** Call AutoLogin on OSS. Return true if AutoLogin started. */
	/** 在 OSS 上调用自动登录功能。如果自动登录已启动，则返回 true。*/
	COMMONUSER_API virtual bool AutoLogin(FOnlineContextCache* System, TSharedRef<FUserLoginRequest> Request, FPlatformUserId PlatformUser);

	/** Call ShowLoginUI on OSS. Return true if ShowLoginUI started. */
	/** 在 OSS 上调用 ShowLoginUI 函数。如果 ShowLoginUI 已启动，则返回 true。*/
	COMMONUSER_API virtual bool ShowLoginUI(FOnlineContextCache* System, TSharedRef<FUserLoginRequest> Request, FPlatformUserId PlatformUser);

	/** Call QueryUserPrivilege on OSS. Return true if QueryUserPrivilege started. */
	/** 调用 OSS 上的“查询用户权限”函数。如果“查询用户权限”操作已开始，则返回 true。*/
	COMMONUSER_API virtual bool QueryUserPrivilege(FOnlineContextCache* System, TSharedRef<FUserLoginRequest> Request, FPlatformUserId PlatformUser);

	/** OSS-specific functions */
	/** 与 OSS 相关的函数 */
#if COMMONUSER_OSSV1
	COMMONUSER_API virtual ECommonUserPrivilege ConvertOSSPrivilege(EUserPrivileges::Type Privilege) const;
	COMMONUSER_API virtual EUserPrivileges::Type ConvertOSSPrivilege(ECommonUserPrivilege Privilege) const;
	COMMONUSER_API virtual ECommonUserPrivilegeResult ConvertOSSPrivilegeResult(EUserPrivileges::Type Privilege, uint32 Results) const;

	COMMONUSER_API void BindOnlineDelegatesOSSv1();
	COMMONUSER_API bool AutoLoginOSSv1(FOnlineContextCache* System, TSharedRef<FUserLoginRequest> Request, FPlatformUserId PlatformUser);
	COMMONUSER_API bool ShowLoginUIOSSv1(FOnlineContextCache* System, TSharedRef<FUserLoginRequest> Request, FPlatformUserId PlatformUser);
	COMMONUSER_API bool QueryUserPrivilegeOSSv1(FOnlineContextCache* System, TSharedRef<FUserLoginRequest> Request, FPlatformUserId PlatformUser);
#else
	COMMONUSER_API virtual ECommonUserPrivilege ConvertOnlineServicesPrivilege(UE::Online::EUserPrivileges Privilege) const;
	COMMONUSER_API virtual UE::Online::EUserPrivileges ConvertOnlineServicesPrivilege(ECommonUserPrivilege Privilege) const;
	COMMONUSER_API virtual ECommonUserPrivilegeResult ConvertOnlineServicesPrivilegeResult(UE::Online::EUserPrivileges Privilege, UE::Online::EPrivilegeResults Results) const;

	COMMONUSER_API void BindOnlineDelegatesOSSv2();
	COMMONUSER_API void CacheConnectionStatus(ECommonUserOnlineContext Context);
	COMMONUSER_API bool TransferPlatformAuthOSSv2(FOnlineContextCache* System, TSharedRef<FUserLoginRequest> Request, FPlatformUserId PlatformUser);
	COMMONUSER_API bool AutoLoginOSSv2(FOnlineContextCache* System, TSharedRef<FUserLoginRequest> Request, FPlatformUserId PlatformUser);
	COMMONUSER_API bool ShowLoginUIOSSv2(FOnlineContextCache* System, TSharedRef<FUserLoginRequest> Request, FPlatformUserId PlatformUser);
	COMMONUSER_API bool QueryUserPrivilegeOSSv2(FOnlineContextCache* System, TSharedRef<FUserLoginRequest> Request, FPlatformUserId PlatformUser);
	COMMONUSER_API TSharedPtr<UE::Online::FAccountInfo> GetOnlineServiceAccountInfo(UE::Online::IAuthPtr AuthService, FPlatformUserId InUserId) const;
#endif

	/** Callbacks for OSS functions */
	/** 用于 OSS 功能的回调函数 */
#if COMMONUSER_OSSV1
	COMMONUSER_API virtual void HandleIdentityLoginStatusChanged(int32 PlatformUserIndex, ELoginStatus::Type OldStatus, ELoginStatus::Type NewStatus, const FUniqueNetId& NewId, ECommonUserOnlineContext Context);
	COMMONUSER_API virtual void HandleUserLoginCompleted(int32 PlatformUserIndex, bool bWasSuccessful, const FUniqueNetId& NetId, const FString& Error, ECommonUserOnlineContext Context);
	COMMONUSER_API virtual void HandleControllerPairingChanged(int32 PlatformUserIndex, FControllerPairingChangedUserInfo PreviousUser, FControllerPairingChangedUserInfo NewUser);
	COMMONUSER_API virtual void HandleNetworkConnectionStatusChanged(const FString& ServiceName, EOnlineServerConnectionStatus::Type LastConnectionStatus, EOnlineServerConnectionStatus::Type ConnectionStatus, ECommonUserOnlineContext Context);
	COMMONUSER_API virtual void HandleOnLoginUIClosed(TSharedPtr<const FUniqueNetId> LoggedInNetId, const int PlatformUserIndex, const FOnlineError& Error, ECommonUserOnlineContext Context);
	COMMONUSER_API virtual void HandleCheckPrivilegesComplete(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults, ECommonUserPrivilege RequestedPrivilege, TWeakObjectPtr<UCommonUserInfo> CommonUserInfo, ECommonUserOnlineContext Context);
#else
	COMMONUSER_API virtual void HandleAuthLoginStatusChanged(const UE::Online::FAuthLoginStatusChanged& EventParameters, ECommonUserOnlineContext Context);
	COMMONUSER_API virtual void HandleUserLoginCompletedV2(const UE::Online::TOnlineResult<UE::Online::FAuthLogin>& Result, FPlatformUserId PlatformUser, ECommonUserOnlineContext Context);
	COMMONUSER_API virtual void HandleOnLoginUIClosedV2(const UE::Online::TOnlineResult<UE::Online::FExternalUIShowLoginUI>& Result, FPlatformUserId PlatformUser, ECommonUserOnlineContext Context);
	COMMONUSER_API virtual void HandleNetworkConnectionStatusChanged(const UE::Online::FConnectionStatusChanged& EventParameters, ECommonUserOnlineContext Context);
	COMMONUSER_API virtual void HandleCheckPrivilegesComplete(const UE::Online::TOnlineResult<UE::Online::FQueryUserPrivilege>& Result, TWeakObjectPtr<UCommonUserInfo> CommonUserInfo, UE::Online::EUserPrivileges DesiredPrivilege, ECommonUserOnlineContext Context);
#endif

	/**
	 * Callback for when an input device (i.e. a gamepad) has been connected or disconnected.
	 * 当输入设备（例如游戏手柄）被连接或断开时的回调函数。
	 * 
	 */
	COMMONUSER_API virtual void HandleInputDeviceConnectionChanged(EInputDeviceConnectionState NewConnectionState, FPlatformUserId PlatformUserId, FInputDeviceId InputDeviceId);

	COMMONUSER_API virtual void HandleLoginForUserInitialize(const UCommonUserInfo* UserInfo, ELoginStatusType NewStatus, FUniqueNetIdRepl NetId, const TOptional<FOnlineErrorType>& Error, ECommonUserOnlineContext Context, FCommonUserInitializeParams Params);
	COMMONUSER_API virtual void HandleUserInitializeFailed(FCommonUserInitializeParams Params, FText Error);
	COMMONUSER_API virtual void HandleUserInitializeSucceeded(FCommonUserInitializeParams Params);

	/** Callback for handling press start/login logic */
	/** 用于处理启动/登录逻辑的回调函数 */
	COMMONUSER_API virtual bool OverrideInputKeyForLogin(FInputKeyEventArgs& EventArgs);


	/** Previous override handler, will restore on cancel */
	/** 上次的覆盖处理程序，取消操作时将恢复该处理程序 */
	FOverrideInputKeyHandler WrappedInputKeyHandler;

	/** List of keys to listen for from any user */
	/** 从任何用户那里监听的键列表 */
	TArray<FKey> LoginKeysForAnyUser;

	/** List of keys to listen for a new unmapped user */
	/** 用于监听新未映射用户的键列表 */
	TArray<FKey> LoginKeysForNewUser;

	/** Params to use for a key-triggered login */
	/** 用于触发基于密钥的登录的参数 */
	FCommonUserInitializeParams ParamsForLoginKey;

	/** Maximum number of local players */
	/** 最多可容纳的本地玩家数量 */
	int32 MaxNumberOfLocalPlayers = 0;
	
	/** True if this is a dedicated server, which doesn't require a LocalPlayer */
	/** 如果这是一个专用服务器（即不需要本地玩家的服务器），则返回 true */
	bool bIsDedicatedServer = false;

	/** List of current in progress login requests */
	/** 当前正在进行的登录请求列表 */
	TArray<TSharedRef<FUserLoginRequest>> ActiveLoginRequests;

	/** Information about each local user, from local player index to user */
	/** 每个本地用户的相关信息，从本地玩家索引到用户本身 */
	UPROPERTY()
	TMap<int32, TObjectPtr<UCommonUserInfo>> LocalUserInfos;
	
	/** Cached platform/mode trait tags */
	/** 缓存的平台/模式特征标签 */
	FGameplayTagContainer CachedTraitTags;

	/** Do not access this outside of initialization */
	/** 在初始化阶段之外请勿访问此内容 */
	FOnlineContextCache* DefaultContextInternal = nullptr;
	FOnlineContextCache* ServiceContextInternal = nullptr;
	FOnlineContextCache* PlatformContextInternal = nullptr;

	friend UCommonUserInfo;
};
