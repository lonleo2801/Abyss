// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Misc/TextFilterExpressionEvaluator.h"

#include "UObject/ObjectPtr.h"
#include "GameSettingFilterState.generated.h"

#define UE_API GAMESETTINGS_API

class ULocalPlayer;
class UGameSetting;
class UGameSettingCollection;

/** Why did the setting change? */
/** 为何设置发生了变化？*/
enum class EGameSettingChangeReason : uint8
{
	Change,
	DependencyChanged,
	ResetToDefault,
	RestoreToInitial,
};

/**
 * The filter state is intended to be any and all filtering we support.
 * 该过滤器状态旨在涵盖我们所支持的所有过滤功能。
 */
USTRUCT()
struct FGameSettingFilterState
{
	GENERATED_BODY()

public:

	UE_API FGameSettingFilterState();

	// 是否包含禁用
	UPROPERTY()
	bool bIncludeDisabled = true;

	// 是否包含隐藏
	UPROPERTY()
	bool bIncludeHidden = false;

	// 是否包含可以重设的,
	// 如果是false则只忽略当前游戏设置状态为不可以重置的.
	// 如果是ture,就是直接通过
	UPROPERTY()
	bool bIncludeResetable = true;

	// 是否包含嵌套页面
	UPROPERTY()
	bool bIncludeNestedPages = false;

public:
	UE_API void SetSearchText(const FString& InSearchText);

	UE_API bool DoesSettingPassFilter(const UGameSetting& InSetting) const;

	// 添加到根游戏设置
	UE_API void AddSettingToRootList(UGameSetting* InSetting);

	// 添加到允许的游戏设置
	UE_API void AddSettingToAllowList(UGameSetting* InSetting);

	bool IsSettingInAllowList(const UGameSetting* InSetting) const
	{
		return SettingAllowList.Contains(InSetting);
	}

	// 获取根设置
	const TArray<UGameSetting*>& GetSettingRootList() const { return SettingRootList; }

	// 是否是根设置
	bool IsSettingInRootList(const UGameSetting* InSetting) const
	{
		return SettingRootList.Contains(InSetting);
	}

private:
	/** 定义了一个表达式评估器，可用于执行复杂的文本筛选查询 */
	FTextFilterExpressionEvaluator SearchTextEvaluator;

	//设置根列表
	UPROPERTY()
	TArray<TObjectPtr<UGameSetting>> SettingRootList;

	// If this is non-empty, then only settings in here are allowed
	// 如果此字段不为空，则仅允许在此处设置的选项生效
	UPROPERTY()
	TArray<TObjectPtr<UGameSetting>> SettingAllowList;
};

/**
 * Editable state captures the current visibility and enabled state of a setting. As well
 * as the reasons it got into that state.
 *
 * “可编辑状态”反映了设置的当前可见性和启用状态。也至于它陷入那种状态的原因。
 */
class FGameSettingEditableState
{
public:
	FGameSettingEditableState()
		: bVisible(true)
		, bEnabled(true)
		, bResetable(true)
		, bHideFromAnalytics(false)
	{
	}
	// 是否可见
	bool IsVisible() const { return bVisible; }
	// 是否可以启用
	bool IsEnabled() const { return bEnabled; }
	// 是否可以恢复至默认
	bool IsResetable() const { return bResetable; }
	// 是否从分析中排除
	bool IsHiddenFromAnalytics() const { return bHideFromAnalytics; }
	// 获取关闭的原因
	const TArray<FText>& GetDisabledReasons() const { return DisabledReasons; }

#if !UE_BUILD_SHIPPING
	// 获取隐藏的原因
	const TArray<FString>& GetHiddenReasons() const { return HiddenReasons; }
#endif
	// 获取关闭的选项
	const TArray<FString>& GetDisabledOptions() const { return DisabledOptions; }

	/** Hides the setting, you don't have to provide a user facing reason, but you do need to specify a developer reason. */
	/** 隐藏该设置，您无需提供面向用户的理由，但必须指明开发者方面的理由。*/
	UE_API void Hide(const FString& DevReason);

	/** Disables the setting, you need to provide a reason you disabled this setting. */
	/** 禁用此设置后，您需要说明为何要禁用此设置。*/
	UE_API void Disable(const FText& Reason);

	/** Discrete Options that should be hidden from the user. Currently used only by Parental Controls. */
	/** 一些应被隐藏起来、不向用户展示的离散选项。目前仅由家长控制功能使用。*/
	UE_API void DisableOption(const FString& Option);

	template<typename EnumType>
	void DisableEnumOption(EnumType InEnumValue)
	{
		DisableOption(StaticEnum<EnumType>()->GetNameStringByValue((int64)InEnumValue));
	}

	/**
	 * Prevents the setting from being reset if the user resets the settings on the screen to their defaults.
	 * 若用户在屏幕上将设置恢复至默认值，则会阻止设置被重新设置。
	 */
	UE_API void UnableToReset();

	/**
	 * Hide from analytics, you may want to do this if for example, we just want to prevent noise, such as platform
	 * specific edit conditions where it doesn't make sense to report settings for platforms where they don't exist.
	 *
	 * 隐藏于分析之外，如果您希望这样做的话（例如，如果我们只是想避免干扰因素，比如平台特定的编辑条件，在这些情况下就不应该为不存在的平台报告设置信息）。
	 */
	void HideFromAnalytics() { bHideFromAnalytics = true; }

	/** Hides it in every way possible.  Hides it visually.  Marks it as Immutable for being reset.  Hides it from analytics. */
	/** 以各种方式将其隐藏起来。 从视觉上将其隐藏。 为防止重置将其标记为不可变。 从分析中将其排除。*/
	void Kill(const FString& DevReason)
	{
		Hide(DevReason);
		HideFromAnalytics();
		UnableToReset();
	}

private:
	uint8 bVisible : 1;
	uint8 bEnabled : 1;
	uint8 bResetable : 1;
	uint8 bHideFromAnalytics : 1;

	TArray<FString> DisabledOptions;

	TArray<FText> DisabledReasons;

#if !UE_BUILD_SHIPPING
	TArray<FString> HiddenReasons;
#endif
};

/**
 * Edit conditions can monitor the state of the game or of other settings and adjust the 
 * visibility.
 *
 * 编辑条件能够监测游戏状态或其他设置的状态，并调整其可视性。
 */
class FGameSettingEditCondition : public TSharedFromThis<FGameSettingEditCondition>
{
public:
	FGameSettingEditCondition() { }
	virtual ~FGameSettingEditCondition() { }

	DECLARE_EVENT_OneParam(FGameSettingEditCondition, FOnEditConditionChanged, bool);
	FOnEditConditionChanged OnEditConditionChangedEvent;

	/** Broadcasts Event*/
	/** 发布事件 */
	void BroadcastEditConditionChanged()
	{
		OnEditConditionChangedEvent.Broadcast(true);
	}

	/** Called during the setting Initialization */
	/** 在设置初始化过程中被调用 */
	virtual void Initialize(const ULocalPlayer* InLocalPlayer)
	{
	}

	/** Called when the setting is 'applied'. */
	/** 当设置被应用时会调用此函数。*/
	virtual void SettingApplied(const ULocalPlayer* InLocalPlayer, UGameSetting* Setting) const
	{
	}

	/** Called when the setting is changed. */
	/** 当设置发生更改时会调用此函数。*/
	virtual void SettingChanged(const ULocalPlayer* InLocalPlayer, UGameSetting* Setting, EGameSettingChangeReason Reason) const
	{
	}

	/**
	 * Called when the setting needs to re-evaluate edit state. Usually this is in response to a 
	 * dependency changing, or if this edit condition emits an OnEditConditionChangedEvent.
	 * 
	 * 当需要重新评估编辑状态时会触发此事件。通常这是由于依赖项发生变化，或者由于此编辑条件发出“编辑条件已更改事件”所导致的。
	 */
	virtual void GatherEditState(const ULocalPlayer* InLocalPlayer, FGameSettingEditableState& InOutEditState) const
	{
	}

	/** Generate useful debugging text for this edit condition.  Helpful when things don't work as expected. */
	/** 生成与此编辑条件相关的有用调试信息。  在出现预期之外的情况时，此功能非常有用。*/
	virtual FString ToString() const { return TEXT(""); }
};

#undef UE_API
