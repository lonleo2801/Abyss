#pragma once
#include "CoreMinimal.h"
//#include "EngineMinimal.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerState.h"
//#include "Core.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineDelegateMacros.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemImpl.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystemUtilsModule.h"
#include "GameFramework/PlayerController.h"
#include "Modules/ModuleManager.h"
#include "OnlineSubsystemUtilsClasses.h"
#include "BlueprintDataDefinitions.generated.h"	

// 用户权限枚举
UENUM(BlueprintType)
enum class EBPUserPrivileges : uint8
{
	/** 无论在线或离线，用户是否可以玩游戏 - 可能受年龄限制 */
	CanPlay,
	/** 用户是否可以在线玩游戏 */
	CanPlayOnline,
	/** 用户是否可以使用语音和文字聊天 */
	CanCommunicateOnline,
	/** 用户是否可以使用其他用户生成的内容 */
	CanUseUserGeneratedContent
};

// 登录状态枚举
UENUM(BlueprintType)
enum class EBPLoginStatus : uint8
{
	/** 玩家尚未登录或选择本地配置文件 */
	NotLoggedIn,
	/** 玩家正在使用本地配置文件但尚未登录 */
	UsingLocalProfile,
	/** 玩家已通过特定平台的身份验证服务验证 */
	LoggedIn
};

// 用户在线账户结构体
USTRUCT(BlueprintType)
struct FBPUserOnlineAccount
{
	GENERATED_USTRUCT_BODY()

public:
	// 用户账户信息共享指针
	TSharedPtr<FUserOnlineAccount> UserAccountInfo;

	FBPUserOnlineAccount()
	{

	}

	FBPUserOnlineAccount(TSharedPtr<FUserOnlineAccount> UserAccount)
	{
		UserAccountInfo = UserAccount;
	}
};

// 会话设置搜索结果枚举
UENUM()
enum class ESessionSettingSearchResult : uint8
{
	// 找到设置
	Found,

	// 未找到设置
	NotFound,

	// 类型不正确
	WrongType
};

// 蓝图结果切换枚举 - 使很多蓝图函数更整洁
UENUM()
enum class EBlueprintResultSwitch : uint8
{
	// 成功
	OnSuccess,

	// 失败
	OnFailure
};

// 蓝图异步结果切换枚举 - 使很多蓝图函数更整洁
UENUM()
enum class EBlueprintAsyncResultSwitch : uint8
{
	// 成功
	OnSuccess,

	// 仍在加载
	AsyncLoading,
	// 失败
	OnFailure
};

// 服务器在线搜索类型定义
UENUM(BlueprintType)
enum class EBPServerPresenceSearchType : uint8
{
	// 所有服务器
	AllServers,
	// 仅客户端服务器
	ClientServersOnly,
	// 仅专用服务器
	DedicatedServersOnly
};

// 在线状态枚举 - 希望在编辑器中可切换
UENUM(BlueprintType)
enum class EBPOnlinePresenceState : uint8
{
	// 在线
	Online,
	// 离线
	Offline,
	// 离开
	Away,
	// 长时间离开
	ExtendedAway,
	// 请勿打扰
	DoNotDisturb,
	// 聊天中
	Chat
};

// 在线会话状态枚举
UENUM(BlueprintType)
enum class EBPOnlineSessionState : uint8
{
	/** 在线会话尚未创建 */
	NoSession,
	/** 正在创建在线会话 */
	Creating,
	/** 会话已创建但尚未开始（赛前大厅） */
	Pending,
	/** 已请求开始会话（由于与后端通信可能需要时间） */
	Starting,
	/** 当前会话已开始。禁用中途加入的会话不再可加入 */
	InProgress,
	/** 会话仍然有效，但不再进行游戏（赛后大厅） */
	Ending,
	/** 会话已关闭并提交了任何统计数据 */
	Ended,
	/** 会话正在被销毁 */
	Destroying
};

// 蓝图唯一的网络ID结构体
// 这是一个比较特殊的处理方式，因为 UniqueNetId 是抽象类，不能轻易重新初始化为共享指针
// 所以将可蓝图化的 UniqueNetID 做成双变量结构体，带有访问函数，并在转换 const 变量为指针
USTRUCT(BlueprintType)
struct FBPUniqueNetId
{
	GENERATED_USTRUCT_BODY()

private:
	// 是否使用直接指针
	bool bUseDirectPointer;


public:
	// 唯一网络ID共享指针
	TSharedPtr<const FUniqueNetId> UniqueNetId;
	// 唯一网络ID原始指针
	const FUniqueNetId * UniqueNetIdPtr;

	// 设置唯一网络ID（通过共享指针）
	void SetUniqueNetId(const TSharedPtr<const FUniqueNetId> &ID)
	{
		bUseDirectPointer = false;
		UniqueNetIdPtr = nullptr;
		UniqueNetId = ID;
	}

	// 设置唯一网络ID（通过原始指针）
	void SetUniqueNetId(const FUniqueNetId *ID)
	{
		bUseDirectPointer = true;
		UniqueNetIdPtr = ID;
	}

	// 检查是否有效
	bool IsValid() const
	{
		if (bUseDirectPointer && UniqueNetIdPtr != nullptr && UniqueNetIdPtr->IsValid())
		{
			return true;
		}
		else if (UniqueNetId.IsValid())
		{
			return true;
		}
		else
			return false;

	}

	// 获取唯一网络ID指针
	const FUniqueNetId* GetUniqueNetId() const
	{
		if (bUseDirectPointer && UniqueNetIdPtr != nullptr)
		{
			// 不再转换为非 const，因为所有函数现在都传递 const UniqueNetIds
			return /*const_cast<FUniqueNetId*>*/(UniqueNetIdPtr);
		}
		else if (UniqueNetId.IsValid())
		{
			return UniqueNetId.Get();
		}
		else
			return nullptr;
	}

	// 添加比较运算符，以便 std 函数可以使用此结构体
	FORCEINLINE bool operator==(const FBPUniqueNetId& Other) const
	{
		return (IsValid() && Other.IsValid() && (*GetUniqueNetId() == *Other.GetUniqueNetId()));
	}

	FORCEINLINE bool operator!=(const FBPUniqueNetId& Other) const
	{
		return !(IsValid() && Other.IsValid() && (*GetUniqueNetId() == *Other.GetUniqueNetId()));
	}

	FBPUniqueNetId()
	{
		bUseDirectPointer = false;
		UniqueNetIdPtr = nullptr;
	}
};

// 蓝图在线用户结构体
USTRUCT(BluePrintType)
struct FBPOnlineUser
{
	GENERATED_USTRUCT_BODY()

public:
	// 唯一网络ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		FBPUniqueNetId UniqueNetId;
	// 显示名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		FString DisplayName;
	// 真实姓名
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		FString RealName;
};

// 蓝图在线最近玩家结构体
USTRUCT(BluePrintType)
struct FBPOnlineRecentPlayer : public FBPOnlineUser
{
	GENERATED_USTRUCT_BODY()

public:
	// 最后一次见到
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		FString LastSeen;
};


// 蓝图好友在线状态信息结构体
USTRUCT(BlueprintType)
struct FBPFriendPresenceInfo
{
	GENERATED_USTRUCT_BODY()

public:

	// 是否在线
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		bool bIsOnline = false;
	// 是否正在游戏
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		bool bIsPlaying = false;
	// 是否正在玩此游戏
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		bool bIsPlayingThisGame = false;
	// 是否可加入
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		bool bIsJoinable = false;
	// 是否支持语音
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		bool bHasVoiceSupport = false;
	// 在线状态
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		EBPOnlinePresenceState PresenceState = EBPOnlinePresenceState::Offline;
	// 状态字符串
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		FString StatusString;

	FBPFriendPresenceInfo()
	{
		bIsOnline = false;
		bIsPlaying = false;
		bIsPlayingThisGame = false;
		bIsJoinable = false;
		bHasVoiceSupport = false;
		PresenceState = EBPOnlinePresenceState::Offline;
	}
};

// 蓝图好友信息结构体
USTRUCT(BlueprintType)
struct FBPFriendInfo
{
	GENERATED_USTRUCT_BODY()

public:

	// 显示名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
	FString DisplayName;
	// 真实姓名
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
	FString RealName;
	// 在线状态
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
	EBPOnlinePresenceState OnlineState = EBPOnlinePresenceState::Offline;
	// 唯一网络ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
	FBPUniqueNetId UniqueNetId;
	// 是否正在玩同一款游戏
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
	bool bIsPlayingSameGame = false;
	// 在线状态信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
	FBPFriendPresenceInfo PresenceInfo;

	FBPFriendInfo()
	{
		OnlineState = EBPOnlinePresenceState::Offline;
		bIsPlayingSameGame = false;
	}
};


/** 给定搜索查询的比较操作类型 */
// 用于比较会话属性
UENUM(BlueprintType)
enum class EOnlineComparisonOpRedux : uint8
{
	// 等于
	Equals,
	// 不等于
	NotEquals,
	// 大于
	GreaterThan,
	// 大于等于
	GreaterThanEquals,
	// 小于
	LessThan,
	// 小于等于
	LessThanEquals,
};


// 用于在转换为 FVariantData 之前存储会话属性
USTRUCT(BlueprintType)
struct FSessionPropertyKeyPair
{
	GENERATED_USTRUCT_BODY()

	// 键名
	FName Key;
	// 数据
	FVariantData Data;
};


// 发送到 FindSessionsAdvanced 以过滤最终结果
USTRUCT(BlueprintType)
struct FSessionsSearchSetting
{
	GENERATED_USTRUCT_BODY()
	//UPROPERTY()


	// 必须复制这个以考虑到原始版本未暴露给蓝图的情况
	/** 此会话设置如何在后端搜索中进行比较 */
	EOnlineComparisonOpRedux ComparisonOp;

	// 要搜索的键值对
	FSessionPropertyKeyPair PropertyKeyPair;
};

// 无法使用默认的，因为它没有暴露给其他模块，必须在这里重新创建
// 各种方法的辅助类，用于减少调用层次
struct FOnlineSubsystemBPCallHelperAdvanced
{
public:
	FOnlineSubsystemBPCallHelperAdvanced(const TCHAR* CallFunctionContext, UWorld* World, FName SystemName = NAME_None)
		: OnlineSub(Online::GetSubsystem(World, SystemName))
		, FunctionContext(CallFunctionContext)
	{
		if (OnlineSub == nullptr)
		{
			FFrame::KismetExecutionMessage(*FString::Printf(TEXT("%s - Invalid or uninitialized OnlineSubsystem"), FunctionContext), ELogVerbosity::Warning);
		}
	}

	void QueryIDFromPlayerController(APlayerController* PlayerController)
	{
		UserID.Reset();
		//return const_cast<FUniqueNetId*>(UniqueNetIdPtr);
		if (APlayerState* PlayerState = (PlayerController != NULL) ? PlayerController->PlayerState : NULL)
		{
			UserID = PlayerState->GetUniqueId().GetUniqueNetId();
			if (!UserID.IsValid())
			{
				FFrame::KismetExecutionMessage(*FString::Printf(TEXT("%s - Cannot map local player to unique net ID"), FunctionContext), ELogVerbosity::Warning);
			}
		}
		else
		{
			FFrame::KismetExecutionMessage(*FString::Printf(TEXT("%s - Invalid player state"), FunctionContext), ELogVerbosity::Warning);
		}
	}


	bool IsValid() const
	{
		return UserID.IsValid() && (OnlineSub != nullptr);
	}

public:
	//TSharedPtr<const FUniqueNetId>& GetUniqueNetId()
	// 用户ID
	TSharedPtr</*class*/ const FUniqueNetId> UserID;
	// 在线子系统指针
	IOnlineSubsystem* const OnlineSub;
	// 函数上下文
	const TCHAR* FunctionContext;
};

// 扩展的在线搜索设置类
class FOnlineSearchSettingsEx : public FOnlineSearchSettings
{
	/**
	*	设置定义搜索参数的键值对组合
	*
	* @param Key 设置的键
	* @param Value 设置的值
	* @param InType 比较类型
	*/
public:

	void HardSet(FName Key, const FVariantData& Value, EOnlineComparisonOpRedux CompOp)
	{
		FOnlineSessionSearchParam* SearchParam = SearchParams.Find(Key);

		TEnumAsByte<EOnlineComparisonOp::Type> op;

		switch (CompOp)
		{
		case EOnlineComparisonOpRedux::Equals: op = EOnlineComparisonOp::Equals; break;
		case EOnlineComparisonOpRedux::GreaterThan: op = EOnlineComparisonOp::GreaterThan; break;
		case EOnlineComparisonOpRedux::GreaterThanEquals: op = EOnlineComparisonOp::GreaterThanEquals; break;
		case EOnlineComparisonOpRedux::LessThan: op = EOnlineComparisonOp::LessThan; break;
		case EOnlineComparisonOpRedux::LessThanEquals: op = EOnlineComparisonOp::LessThanEquals; break;
		case EOnlineComparisonOpRedux::NotEquals: op = EOnlineComparisonOp::NotEquals; break;
		default: op = EOnlineComparisonOp::Equals; break;
		}

		if (SearchParam)
		{
			SearchParam->Data = Value;
			SearchParam->ComparisonOp = op;
		}
		else
		{
			FOnlineSessionSearchParam searchSetting((int)0, op);
			searchSetting.Data = Value;
			SearchParams.Add(Key, searchSetting);
		}
	}
};

#define INVALID_INDEX -1