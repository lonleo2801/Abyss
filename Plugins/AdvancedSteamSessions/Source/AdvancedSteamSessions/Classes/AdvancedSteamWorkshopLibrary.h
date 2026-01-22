// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#if (PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX) && STEAM_SDK_INSTALLED
#include "steam/isteamugc.h"
#include "steam/isteamremotestorage.h"
#endif
#include "Interfaces/OnlineSessionInterface.h"

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


#pragma pop_macro("ARRAY_COUNT")

#endif

// @todo Steam: See above
#ifdef _MSC_VER
#pragma warning(pop)
#endif


#include "AdvancedSteamWorkshopLibrary.generated.h"


// 一般高级 Steam 创意工坊日志
DECLARE_LOG_CATEGORY_EXTERN(AdvancedSteamWorkshopLog, Log, All);


// 使用自定义结构体是因为 uint32 不支持蓝图，并且我不想转换为 int32
// 因为创意工坊的大小可能会导致溢出？
USTRUCT(BlueprintType)
struct FBPSteamWorkshopID
{
	GENERATED_USTRUCT_BODY()

public:

	uint64 SteamWorkshopID;

	FBPSteamWorkshopID()
	{

	}

	FBPSteamWorkshopID(uint64 ID)
	{
		SteamWorkshopID = ID;
	}
};


// 一般结果代码 - 复制 Steam 版本
// 检查这些以适应未来变化
UENUM(BlueprintType)
enum class FBPSteamResult : uint8
{
	K_EResultInvalid = 0,
	k_EResultOK = 1,							// 成功
	k_EResultFail = 2,							// 通用失败
	k_EResultNoConnection = 3,					// 无/失败的网络连接
	//	k_EResultNoConnectionRetry = 4,				// 已废弃 - 删除
	k_EResultInvalidPassword = 5,				// 密码/票证无效
	k_EResultLoggedInElsewhere = 6,				// 同一用户在其他地方登录
	k_EResultInvalidProtocolVer = 7,			// 协议版本不正确
	k_EResultInvalidParam = 8,					// 参数不正确
	k_EResultFileNotFound = 9,					// 文件未找到
	k_EResultBusy = 10,							// 调用的方法繁忙 - 未采取操作
	k_EResultInvalidState = 11,					// 调用对象处于无效状态
	k_EResultInvalidName = 12,					// 名称无效
	k_EResultInvalidEmail = 13,					// 电子邮件无效
	k_EResultDuplicateName = 14,				// 名称不唯一
	k_EResultAccessDenied = 15,					// 访问被拒绝
	k_EResultTimeout = 16,						// 操作超时
	k_EResultBanned = 17,						// VAC2 封禁
	k_EResultAccountNotFound = 18,				// 账户未找到
	k_EResultInvalidSteamID = 19,				// SteamID 无效
	k_EResultServiceUnavailable = 20,			// 请求的服务当前不可用
	k_EResultNotLoggedOn = 21,					// 用户未登录
	k_EResultPending = 22,						// 请求挂起（可能正在处理中，或等待第三方）
	k_EResultEncryptionFailure = 23,			// 加密或解密失败
	k_EResultInsufficientPrivilege = 24,		// 权限不足
	k_EResultLimitExceeded = 25,				// 超过限制
	k_EResultRevoked = 26,						// 访问已被撤销（用于撤销的访客通行证）
	k_EResultExpired = 27,						// 用户尝试访问的许可证/访客通行证已过期
	k_EResultAlreadyRedeemed = 28,				// 访客通行证已被账户兑换，无法再次确认
	k_EResultDuplicateRequest = 29,				// 请求是重复的，并且操作在过去已经发生过，这次被忽略
	k_EResultAlreadyOwned = 30,					// 此次访客通行证兑换请求中的所有游戏均已被用户拥有
	k_EResultIPNotFound = 31,					// IP地址未找到
	k_EResultPersistFailed = 32,				// 写入数据存储更改失败
	k_EResultLockingFailed = 33,				// 获取此操作的访问锁失败
	k_EResultLogonSessionReplaced = 34,
	k_EResultConnectFailed = 35,
	k_EResultHandshakeFailed = 36,
	k_EResultIOFailure = 37,
	k_EResultRemoteDisconnect = 38,
	k_EResultShoppingCartNotFound = 39,			// 未找到请求的购物车
	k_EResultBlocked = 40,						// 用户不允许
	k_EResultIgnored = 41,						// 目标忽略发送者
	k_EResultNoMatch = 42,						// 未找到匹配请求的内容
	k_EResultAccountDisabled = 43,
	k_EResultServiceReadOnly = 44,				// 此服务目前不接受内容更改
	k_EResultAccountNotFeatured = 45,			// 账户没有价值，因此此功能不可用
	k_EResultAdministratorOK = 46,				// 允许执行此操作，但仅因为请求者是管理员
	k_EResultContentVersion = 47,				// Steam 协议中传输的内容版本不匹配。
	k_EResultTryAnotherCM = 48,					// 当前 CM 无法为发出请求的用户提供服务，用户应尝试其他 CM。
	k_EResultPasswordRequiredToKickSession = 49,// 您已在其他地方登录，此缓存凭据登录失败。
	k_EResultAlreadyLoggedInElsewhere = 50,		// 您已在其他地方登录，必须等待
	k_EResultSuspended = 51,					// 长时间运行的操作（内容下载）已暂停
	k_EResultCancelled = 52,					// 操作取消（通常由用户取消：内容下载）
	k_EResultDataCorruption = 53,				// 操作取消，因为数据格式错误或无法恢复
	k_EResultDiskFull = 54,						// 操作取消 - 磁盘空间不足。
	k_EResultRemoteCallFailed = 55,				// 远程调用或 IPC 调用失败
	k_EResultPasswordUnset = 56,				// 密码无法验证，因为它在服务器端未设置
	k_EResultExternalAccountUnlinked = 57,		// 外部账户（PSN, Facebook...）未链接到 Steam 账户
	k_EResultPSNTicketInvalid = 58,				// PSN 票证无效
	k_EResultExternalAccountAlreadyLinked = 59,	// 外部账户（PSN, Facebook...）已链接到其他账户，必须先明确请求替换/删除链接
	k_EResultRemoteFileConflict = 60,			// 由于本地和远程文件冲突，同步无法恢复
	k_EResultIllegalPassword = 61,				// 请求的新密码不合法
	k_EResultSameAsPreviousValue = 62,			// 新值与旧值相同（密保问题和答案）
	k_EResultAccountLogonDenied = 63,			// 由于双因素身份验证失败，账户登录被拒绝
	k_EResultCannotUseOldPassword = 64,			// 请求的新密码不合法
	k_EResultInvalidLoginAuthCode = 65,			// 由于验证码无效，账户登录被拒绝
	k_EResultAccountLogonDeniedNoMail = 66,		// 由于双因素身份验证失败，账户登录被拒绝 - 且未发送邮件
	k_EResultHardwareNotCapableOfIPT = 67,		// 
	k_EResultIPTInitError = 68,					// 
	k_EResultParentalControlRestricted = 69,	// 由于当前用户的家长控制限制，操作失败
	k_EResultFacebookQueryError = 70,			// Facebook 查询返回错误
	k_EResultExpiredLoginAuthCode = 71,			// 由于验证码过期，账户登录被拒绝
	k_EResultIPLoginRestrictionFailed = 72,
	k_EResultAccountLockedDown = 73,
	k_EResultAccountLogonDeniedVerifiedEmailRequired = 74,
	k_EResultNoMatchingURL = 75,
	k_EResultBadResponse = 76,					// 解析失败，缺少字段等
	k_EResultRequirePasswordReEntry = 77,		// 用户必须重新输入密码才能完成操作
	k_EResultValueOutOfRange = 78,				// 输入的值超出可接受范围
	k_EResultUnexpectedError = 79,				// 发生了我们要预料之外的事情
	k_EResultDisabled = 80,						// 请求的服务已被配置为不可用
	k_EResultInvalidCEGSubmission = 81,			// 提交给 CEG 服务器的文件集无效！
	k_EResultRestrictedDevice = 82,				// 使用的设备不允许执行此操作
	k_EResultRegionLocked = 83,					// 由于区域限制，无法完成操作
	k_EResultRateLimitExceeded = 84,			// 超过临时速率限制，稍后再试，不同于可能永久的 k_EResultLimitExceeded
	k_EResultAccountLoginDeniedNeedTwoFactor = 85,	// 需要双因素代码才能登录
	k_EResultItemDeleted = 86,					// 我们尝试访问的内容已被删除
	k_EResultAccountLoginDeniedThrottle = 87,	// 登录尝试失败，尝试对可能的攻击者进行限流响应
	k_EResultTwoFactorCodeMismatch = 88,		// 双因素代码不匹配
	k_EResultTwoFactorActivationCodeMismatch = 89,	// 双因素激活码不匹配
	k_EResultAccountAssociatedToMultiplePartners = 90,	// 账户已与多个合作伙伴关联
	k_EResultNotModified = 91, // 数据未修改
};

// 检查这些以适应未来变化
UENUM(BlueprintType)
enum class FBPWorkshopFileType : uint8
{
	k_EWorkshopFileTypeCommunity = 0,
	k_EWorkshopFileTypeMicrotransaction = 1,
	k_EWorkshopFileTypeCollection = 2,
	k_EWorkshopFileTypeArt = 3,
	k_EWorkshopFileTypeVideo = 4,
	k_EWorkshopFileTypeScreenshot = 5,
	k_EWorkshopFileTypeGame = 6,
	k_EWorkshopFileTypeSoftware = 7,
	k_EWorkshopFileTypeConcept = 8,
	k_EWorkshopFileTypeWebGuide = 9,
	k_EWorkshopFileTypeIntegratedGuide = 10,
	k_EWorkshopFileTypeMerch = 11,
	k_EWorkshopFileTypeControllerBinding = 12,
	k_EWorkshopFileTypeSteamworksAccessInvite = 13,
	k_EWorkshopFileTypeSteamVideo = 14,

	// 如果添加值，请更新 k_EWorkshopFileTypeMax
	k_EWorkshopFileTypeMax = 15
};

// 创意工坊物品详情结构体
USTRUCT(BlueprintType)
struct FBPSteamWorkshopItemDetails
{
	GENERATED_USTRUCT_BODY()

public:

	FBPSteamWorkshopItemDetails()
	{
		ResultOfRequest = FBPSteamResult::k_EResultOK;
		FileType = FBPWorkshopFileType::k_EWorkshopFileTypeMax;
		CreatorAppID = 0;
		ConsumerAppID = 0;
		VotesUp = 0;
		VotesDown = 0;
		CalculatedScore = 0.f;
		bBanned = false;
		bAcceptedForUse = false;
		bTagsTruncated = false;
	}

#if (PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX) && STEAM_SDK_INSTALLED
	FBPSteamWorkshopItemDetails(SteamUGCDetails_t &hUGCDetails)
	{
		ResultOfRequest = (FBPSteamResult)hUGCDetails.m_eResult;
		FileType = (FBPWorkshopFileType)hUGCDetails.m_eFileType;
		CreatorAppID = (int32)hUGCDetails.m_nCreatorAppID;
		ConsumerAppID = (int32)hUGCDetails.m_nConsumerAppID;
		Title = FString(hUGCDetails.m_rgchTitle, k_cchPublishedDocumentTitleMax);
		Description = FString(hUGCDetails.m_rgchDescription, k_cchPublishedDocumentDescriptionMax);
		ItemUrl = FString(hUGCDetails.m_rgchURL, k_cchPublishedFileURLMax);
		VotesUp = (int32)hUGCDetails.m_unVotesUp;
		VotesDown = (int32)hUGCDetails.m_unVotesDown;
		CalculatedScore = hUGCDetails.m_flScore;
		bBanned = hUGCDetails.m_bBanned;
		bAcceptedForUse = hUGCDetails.m_bAcceptedForUse;
		bTagsTruncated = hUGCDetails.m_bTagsTruncated;

		CreatorSteamID = FString::Printf(TEXT("%llu"), hUGCDetails.m_ulSteamIDOwner);
	}

	FBPSteamWorkshopItemDetails(const SteamUGCDetails_t &hUGCDetails)
	{
		ResultOfRequest = (FBPSteamResult)hUGCDetails.m_eResult;
		FileType = (FBPWorkshopFileType)hUGCDetails.m_eFileType;
		CreatorAppID = (int32)hUGCDetails.m_nCreatorAppID;
		ConsumerAppID = (int32)hUGCDetails.m_nConsumerAppID;
		Title = FString(hUGCDetails.m_rgchTitle, k_cchPublishedDocumentTitleMax);
		Description = FString(hUGCDetails.m_rgchDescription, k_cchPublishedDocumentDescriptionMax);
		ItemUrl = FString(hUGCDetails.m_rgchURL, k_cchPublishedFileURLMax);
		VotesUp = (int32)hUGCDetails.m_unVotesUp;
		VotesDown = (int32)hUGCDetails.m_unVotesDown;
		CalculatedScore = hUGCDetails.m_flScore;
		bBanned = hUGCDetails.m_bBanned;
		bAcceptedForUse = hUGCDetails.m_bAcceptedForUse;
		bTagsTruncated = hUGCDetails.m_bTagsTruncated;

		CreatorSteamID = FString::Printf(TEXT("%llu"), hUGCDetails.m_ulSteamIDOwner);
	}
#endif

	// 获取详细信息的结果
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
		FBPSteamResult ResultOfRequest;

	// 文件类型
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
		FBPWorkshopFileType FileType;

	// 这两个列为 int，但存储为 uint，我认为保持 int 是安全的
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
		int32 CreatorAppID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
		int32 ConsumerAppID;

	// 物品标题
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
		FString Title;

	// 物品描述
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
		FString Description;
	
	// 网站视频的URL
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
		FString ItemUrl;

	// 投票不太可能超过有符号限制
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
	int32 VotesUp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
	int32 VotesDown;

	// 计算得分
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
	float CalculatedScore;

	// 文件是否被封禁
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
	bool bBanned;													
	
	// 开发人员已明确标记此项目在创意工坊中被接受
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
	bool bAcceptedForUse;	
	
	// 标签列表是否太长而无法在提供的缓冲区中返回
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
	bool bTagsTruncated;

	// 创建此内容的用户的 Steam ID。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
	FString CreatorSteamID;

	/*
	PublishedFileId_t m_nPublishedFileId;
	uint32 m_rtimeCreated;											// time when the published file was created
	uint32 m_rtimeUpdated;											// time when the published file was last updated
	uint32 m_rtimeAddedToUserList;									// time when the user added the published file to their list (not always applicable)
	ERemoteStoragePublishedFileVisibility m_eVisibility;			// visibility
	char m_rgchTags[k_cchTagListMax];								// comma separated list of all tags associated with this file
	// file/url information
	UGCHandle_t m_hFile;											// The handle of the primary file
	UGCHandle_t m_hPreviewFile;										// The handle of the preview file
	char m_pchFileName[k_cchFilenameMax];							// The cloud filename of the primary file
	int32 m_nFileSize;												// Size of the primary file
	int32 m_nPreviewFileSize;										// Size of the preview file
	uint32 m_unNumChildren;											// if m_eFileType == k_EWorkshopFileTypeCollection, then this number will be the number of children contained within the collection
	*/

};

UCLASS()
class UAdvancedSteamWorkshopLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	//********* Steam Functions *************//
	
	// 返回订阅的创意工坊物品ID，TArray 长度决定了有多少个
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSteamWorkshop")
	static TArray<FBPSteamWorkshopID> GetSubscribedWorkshopItems(int32 & NumberOfItems);

	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSteamWorkshop")
	static void GetNumSubscribedWorkshopItems(int32 & NumberOfItems);

};	
