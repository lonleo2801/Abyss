// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
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
#include "BlueprintDataDefinitions.h"
#include "UObject/UObjectIterator.h"

// This is taken directly from UE4 - OnlineSubsystemSteamPrivatePCH.h as a fix for the array_count macro
// @todo Steam: Steam headers trigger secure-C-runtime warnings in Visual C++. Rather than mess with _CRT_SECURE_NO_WARNINGS, we'll just
//	disable the warnings locally. Remove when this is fixed in the SDK
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
// #TODO check back on this at some point
#pragma warning(disable:4265) // SteamAPI CCallback< specifically, this warning is off by default but 4.17 turned it on....
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX) && STEAM_SDK_INSTALLED

#pragma push_macro("ARRAY_COUNT")
#undef ARRAY_COUNT

#if USING_CODE_ANALYSIS
MSVC_PRAGMA(warning(push))
MSVC_PRAGMA(warning(disable : ALL_CODE_ANALYSIS_WARNINGS))
#endif	// USING_CODE_ANALYSIS

#include <steam/steam_api.h>

#if USING_CODE_ANALYSIS
MSVC_PRAGMA(warning(pop))
#endif	// USING_CODE_ANALYSIS

#include <steam/isteamapps.h>
//#include <steam/isteamapplist.h>
//#include <OnlineSubsystemSteamTypes.h>
#pragma pop_macro("ARRAY_COUNT")

// @todo Steam: See above
#ifdef _MSC_VER
#pragma warning(pop)
#endif

// Making a copy of this here since the original is still in a private folder and is screwing with things
/**
* Steam specific implementation of the unique net id
* 唯一网络ID的Steam特定实现
*/
class FUniqueNetIdSteam2 :
	public FUniqueNetId
{
PACKAGE_SCOPE:
	/** Holds the net id for a player */
	/** 保存玩家的网络ID */
	uint64 UniqueNetId;

	/** Hidden on purpose */
	/** 故意隐藏 */
	FUniqueNetIdSteam2() :
		UniqueNetId(0)
	{
	}

	/**
	* Copy Constructor
	* 拷贝构造函数
	*
	* @param Src the id to copy
	*/
	explicit FUniqueNetIdSteam2(const FUniqueNetIdSteam2& Src) :
		UniqueNetId(Src.UniqueNetId)
	{
	}

public:
	/**
	* Constructs this object with the specified net id
	* 使用指定的网络ID构造此对象
	*
	* @param InUniqueNetId the id to set ours to
	*/
	explicit FUniqueNetIdSteam2(uint64 InUniqueNetId) :
		UniqueNetId(InUniqueNetId)
	{
	}

	/**
	* Constructs this object with the steam id
	* 使用Steam ID构造此对象
	*
	* @param InUniqueNetId the id to set ours to
	*/
	explicit FUniqueNetIdSteam2(CSteamID InSteamId) :
		UniqueNetId(InSteamId.ConvertToUint64())
	{
	}

	/**
	* Constructs this object with the specified net id
	* 使用指定的网络ID构造此对象
	*
	* @param String textual representation of an id
	*/
	explicit FUniqueNetIdSteam2(const FString& Str) :
		UniqueNetId(FCString::Atoi64(*Str))
	{
	}


	/**
	* Constructs this object with the specified net id
	* 使用指定的网络ID构造此对象
	*
	* @param InUniqueNetId the id to set ours to (assumed to be FUniqueNetIdSteam in fact)
	*/
	explicit FUniqueNetIdSteam2(const FUniqueNetId& InUniqueNetId) :
		UniqueNetId(*(uint64*)InUniqueNetId.GetBytes())
	{
	}

	virtual FName GetType() const override
	{
		return STEAM_SUBSYSTEM;
	}

	/**
	* Get the raw byte representation of this net id
	* This data is platform dependent and shouldn't be manipulated directly
	* 获取此网络ID的原始字节表示
	* 此数据依赖于平台，不应直接操作
	*
	* @return byte array of size GetSize()
	*/
	virtual const uint8* GetBytes() const override
	{
		return (uint8*)&UniqueNetId;
	}

	/**
	* Get the size of the id
	* 获取ID的大小
	*
	* @return size in bytes of the id representation
	*/
	virtual int32 GetSize() const override
	{
		return sizeof(uint64);
	}

	/**
	* Check the validity of the id
	* 检查ID的有效性
	*
	* @return true if this is a well formed ID, false otherwise
	*/
	virtual bool IsValid() const override
	{
		return UniqueNetId != 0 && CSteamID(UniqueNetId).IsValid();
	}

	/**
	* Platform specific conversion to string representation of data
	* 平台特定的数据字符串转换
	*
	* @return data in string form
	*/
	virtual FString ToString() const override
	{
		return FString::Printf(TEXT("%llu"), UniqueNetId);
	}

	/**
	* Get a human readable representation of the net id
	* Shouldn't be used for anything other than logging/debugging
	* 获取网络ID的人类可读表示
	* 仅用于日志记录/调试
	*
	* @return id in string form
	*/
	virtual FString ToDebugString() const override
	{
		CSteamID SteamID(UniqueNetId);
		if (SteamID.IsLobby())
		{
			return FString::Printf(TEXT("Lobby [0x%llX]"), UniqueNetId);
		}
		else if (SteamID.BAnonGameServerAccount())
		{
			return FString::Printf(TEXT("Server [0x%llX]"), UniqueNetId);
		}
		else if (SteamID.IsValid())
		{
			const FString NickName(SteamFriends() ? UTF8_TO_TCHAR(SteamFriends()->GetFriendPersonaName(UniqueNetId)) : TEXT("UNKNOWN"));
			return FString::Printf(TEXT("%s [0x%llX]"), *NickName, UniqueNetId);
		}
		else
		{
			return FString::Printf(TEXT("INVALID [0x%llX]"), UniqueNetId);
		}
	}


	virtual uint32 GetTypeHash() const override
	{
		return ::GetTypeHash(UniqueNetId);
	}

	/** Convenience cast to CSteamID */
	operator CSteamID()
	{
		return UniqueNetId;
	}

	/** Convenience cast to CSteamID */
	operator const CSteamID() const
	{
		return UniqueNetId;
	}

	/** Convenience cast to CSteamID pointer */
	operator CSteamID*()
	{
		return (CSteamID*)&UniqueNetId;
	}

	/** Convenience cast to CSteamID pointer */
	operator const CSteamID*() const
	{
		return (const CSteamID*)&UniqueNetId;
	}

	friend FArchive& operator<<(FArchive& Ar, FUniqueNetIdSteam2& UserId)
	{
		return Ar << UserId.UniqueNetId;
	}
};

#endif

#include "AdvancedSteamFriendsLibrary.generated.h"


// 一般高级 Steam 好友日志
DECLARE_LOG_CATEGORY_EXTERN(AdvancedSteamFriendsLog, Log, All);

// Steam头像大小枚举
UENUM(Blueprintable)
enum class SteamAvatarSize : uint8
{
	SteamAvatar_INVALID = 0,
	SteamAvatar_Small = 1,
	SteamAvatar_Medium = 2,
	SteamAvatar_Large = 3
};

// Steam用户覆盖类型枚举
UENUM(Blueprintable)
enum class ESteamUserOverlayType : uint8
{
	/*打开覆盖网页浏览器到指定用户或组的个人资料。*/
	steamid,
	/*打开与指定用户的聊天窗口，或加入群组聊天。*/
	chat,
	/*打开使用 ISteamEconomy / StartTrade Web API 启动的 Steam 交易会话窗口。*/
	jointrade,
	/*打开覆盖网页浏览器到指定用户的统计数据。*/
	stats,
	/*打开覆盖网页浏览器到指定用户的成就。*/
	achievements,
	/*以最小模式打开覆盖，提示用户添加目标用户为好友。*/
	friendadd,
	/*以最小模式打开覆盖，提示用户删除目标好友。*/
	friendremove,
	/*以最小模式打开覆盖，提示用户接受传入的好友邀请。*/
	friendrequestaccept,
	/*以最小模式打开覆盖，提示用户忽略传入的好友邀请。*/
	friendrequestignore,
	/*打开邀请覆盖，从此对话框发送的邀请将针对提供的大厅*/
	invitetolobby
};

static FString EnumToString(const FString& enumName, uint8 value)
{
	
	const UEnum* EnumPtr = FindFirstObject<UEnum>(*enumName, EFindFirstObjectOptions::None, ELogVerbosity::Warning, TEXT("EumtoString"));

	if (!EnumPtr)
		return FString();

	FString EnumName = EnumPtr->GetNameStringByIndex(value);
	return EnumName;
}


// Steam群组信息结构体
USTRUCT(BlueprintType, Category = "Online|SteamAPI|SteamGroups")
struct FBPSteamGroupInfo
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|SteamAPI|SteamGroups")
		FBPUniqueNetId GroupID; // Uint64 representation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|SteamAPI|SteamGroups")
		FString GroupName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|SteamAPI|SteamGroups")
		FString GroupTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|SteamAPI|SteamGroups")
		int32 numOnline = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|SteamAPI|SteamGroups")
		int32 numInGame = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|SteamAPI|SteamGroups")
		int32 numChatting = 0;

};

// 文本过滤上下文枚举
UENUM(Blueprintable)
enum class EBPTextFilteringContext : uint8
{
	/*未知上下文。*/
	FContext_Unknown = 0,
	/*游戏内容，仅执行法律要求的过滤。*/
	FContext_GameContent = 1,
	/*来自其他玩家的聊天。*/
	FContext_Chat = 2,
	/*角色或物品名称。*/
	FContext_Name = 3
};

UCLASS()
class UAdvancedSteamFriendsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	//********* 好友列表函数 *************//

	// 获取有效好友头像的纹理，仅限STEAM，如果子系统尚未加载该大小的头像，则返回无效纹理
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|SteamAPI", meta = (ExpandEnumAsExecs = "Result"))
	static UTexture2D * GetSteamFriendAvatar(const FBPUniqueNetId UniqueNetId, EBlueprintAsyncResultSwitch &Result, SteamAvatarSize AvatarSize = SteamAvatarSize::SteamAvatar_Medium);

	// 预加载Steam好友的头像和名称，返回是否已可用，仅限STEAM，调用后需要时间实际加载所有内容。
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|SteamAPI")
	static bool RequestSteamFriendInfo(const FBPUniqueNetId UniqueNetId, bool bRequireNameOnly = false);

	// 打开Steam覆盖层以转到指定的用户对话框
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|SteamAPI", meta = (WorldContext = "WorldContextObject"))
		static bool OpenSteamUserOverlay(UObject* WorldContextObject, const FBPUniqueNetId UniqueNetId, ESteamUserOverlayType DialogType);
	
	// 返回Steam覆盖层当前是否处于活动状态（在初始覆盖挂钩期间可能返回false）
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedFriends|SteamAPI")
		static bool IsOverlayEnabled();

	// 获取好友Steam账户的等级，仅限STEAM，如果不知道Steam等级则返回-1，可能需要先调用 RequestSteamFriendInfo。
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|SteamAPI")
	static int32 GetFriendSteamLevel(const FBPUniqueNetId UniqueNetId);

	// 获取Steam ID的个人名称，仅限STEAM，如果没有结果则返回空，可能需要先调用 RequestSteamFriendInfo。
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|SteamAPI")
	static FString GetSteamPersonaName(const FBPUniqueNetId UniqueNetId);

	// 直接从保存uint64值的字符串创建唯一的Steam ID，用于测试
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedFriends|SteamAPI")
	static FBPUniqueNetId CreateSteamIDFromString(const FString SteamID64);

	// 从Steam获取本地Steam ID
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedFriends|SteamAPI")
		static FBPUniqueNetId GetLocalSteamIDFromSteam();

	/* 获取好友当前正在玩的游戏 - AppID 是 int32 即使 steam id 是 uint32，目前在蓝图中无法避免
	*  可以将 AppID 与 WebAPI GetAppList 请求一起使用。
	*/
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|SteamAPI", meta = (ExpandEnumAsExecs = "Result"))
	static void GetSteamFriendGamePlayed(const FBPUniqueNetId UniqueNetId, EBlueprintResultSwitch &Result/*, FString & GameName*/, int32 & AppID);

	// 获取Steam群组的完整列表
	UFUNCTION(BlueprintCallable, Category = "Online|SteamAPI|SteamGroups")
		static void GetSteamGroups(TArray<FBPSteamGroupInfo> & SteamGroups);

	// 初始化文本过滤（预加载字典）
	// 如果成功返回true，如果游戏语言不可用过滤则返回false
	UFUNCTION(BlueprintCallable, Category = "Online|SteamAPI|TextFiltering")
		static bool InitTextFiltering();

	// 尝试使用给定的过滤上下文过滤字符串
	// 如果文本已被过滤返回true，如果未过滤（无需过滤或操作失败）返回false
	// 如果为false，它仍将输出原始文本
	// Textsource 是作为文本来源（玩家名称/聊天）的Steam ID
	// 需要先调用 InitTextFiltering！！
	UFUNCTION(BlueprintCallable, Category = "Online|SteamAPI|TextFiltering")
		static bool FilterText(FString TextToFilter, EBPTextFilteringContext Context, const FBPUniqueNetId TextSourceID, FString& FilteredText);

	// 返回Steam是否在大屏幕模式下运行
	UFUNCTION(BlueprintPure, Category = "Online|SteamAPI")
		static bool IsSteamInBigPictureMode();
};	
