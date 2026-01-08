// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Components/SlateWrapperTypes.h"
#include "GameSettingFilterState.h"
#include "GameplayTagContainer.h"

#include "GameSetting.generated.h"

#define UE_API GAMESETTINGS_API

class ULocalPlayer;
class UGameSettingRegistry;

//--------------------------------------
// UGameSetting
//--------------------------------------

DECLARE_DELEGATE_RetVal_OneParam(FText, FGetGameSettingsDetails, ULocalPlayer& /*InLocalPlayer*/);

/**
 * 
 */
UCLASS(MinimalAPI, Abstract, BlueprintType)
class UGameSetting : public UObject
{
	GENERATED_BODY()

public:
	UGameSetting() { }

public:
	DECLARE_EVENT_TwoParams(UGameSetting, FOnSettingChanged, UGameSetting* /*InSetting*/, EGameSettingChangeReason /*InChangeReason*/);
	DECLARE_EVENT_OneParam(UGameSetting, FOnSettingApplied, UGameSetting* /*InSetting*/);
	DECLARE_EVENT_OneParam(UGameSetting, FOnSettingEditConditionChanged, UGameSetting* /*InSetting*/);

	// 游戏设置值改变
	FOnSettingChanged OnSettingChangedEvent;
	// 游戏设置被被应用
	FOnSettingApplied OnSettingAppliedEvent;
	// 游戏设置的编辑条件发生改变
	FOnSettingEditConditionChanged OnSettingEditConditionChangedEvent;

public:

	/**
	 * Gets the non-localized developer name for this setting.  This should remain constant, and represent a 
	 * unique identifier for this setting inside this settings registry.
	 */
	/**
	 *
	 * 获取此设置的非本地化开发者名称。此名称应保持不变，并代表此设置在该设置注册表中的唯一标识符。
	 */
	UFUNCTION(BlueprintCallable)
	FName GetDevName() const { return DevName; }
	// 设置开发者名称
	void SetDevName(const FName& Value) { DevName = Value; }

	bool GetAdjustListViewPostRefresh() const { return bAdjustListViewPostRefresh; }
	void SetAdjustListViewPostRefresh(const bool Value) { bAdjustListViewPostRefresh = Value; }

	// 设置本地化名称
	UFUNCTION(BlueprintCallable)
	FText GetDisplayName() const { return DisplayName; }
	void SetDisplayName(const FText& Value) { DisplayName = Value; }
#if !UE_BUILD_SHIPPING
	void SetDisplayName(const FString& Value) { SetDisplayName(FText::FromString(Value)); }
#endif
	UFUNCTION(BlueprintCallable)
	ESlateVisibility GetDisplayNameVisibility() { return DisplayNameVisibility; }
	void SetNameDisplayVisibility(ESlateVisibility InVisibility) { DisplayNameVisibility = InVisibility; }

	// 设置富文本描述
	UFUNCTION(BlueprintCallable)
	FText GetDescriptionRichText() const { return DescriptionRichText; }
	void SetDescriptionRichText(const FText& Value) { DescriptionRichText = Value; InvalidateSearchableText(); }
#if !UE_BUILD_SHIPPING
	/** This version is for cheats and other non-shipping items, that don't need to localize their text.  We don't permit this in shipping to prevent unlocalized text being introduced. */
	/** 此版本适用于作弊道具及其他非发货商品，这类商品无需对文本进行本地化处理。在发货环节中我们不允许采用此方式，以避免引入未本地化的文本内容。*/
	void SetDescriptionRichText(const FString& Value) { SetDescriptionRichText(FText::FromString(Value)); }
#endif

	UFUNCTION(BlueprintCallable)
	const FGameplayTagContainer& GetTags() const { return Tags; }
	void AddTag(const FGameplayTag& TagToAdd) { Tags.AddTag(TagToAdd); }

	// 设置注册器
	void SetRegistry(UGameSettingRegistry* InOwningRegistry) { OwningRegistry = InOwningRegistry; }

	/** Gets the searchable plain text for the description. */
	/** 获取描述内容的可搜索纯文本内容。*/
	UE_API const FString& GetDescriptionPlainText() const;

	/** Initializes the setting, giving it the owning local player.  Containers automatically initialize settings added to them. */
	/** 初始化设置，并将本地玩家对象作为其所属对象。  容器会自动初始化添加到其之中的设置。*/
	UE_API void Initialize(ULocalPlayer* InLocalPlayer);

	/** Gets the owning local player for this setting - which all initialized settings will have. */
	/** 获取此设置所对应的拥有者本地玩家对象——所有已初始化的设置都会具有此属性。*/
	ULocalPlayer* GetOwningLocalPlayer() const { return LocalPlayer; }
	
	/** Set the dynamic details callback, we query this when building the description panel.  This text is not searchable.*/
	/** 设置动态详情回调函数，我们在构建描述面板时会调用此函数。此文本不支持搜索。*/
	void SetDynamicDetails(const FGetGameSettingsDetails& InDynamicDetails) { DynamicDetails = InDynamicDetails; }

	/**
	 * Gets the dynamic details about this setting.  This may be information like, how many refunds are remaining 
	 * on their account, or the account number.
	 *
	 * 获取有关此设置的动态详细信息。这可能包括诸如账户中剩余的退款数量，或者账户号码等信息。
	 * 
	 */
	UFUNCTION(BlueprintCallable)
	UE_API FText GetDynamicDetails() const;

	UFUNCTION(BlueprintCallable)
	FText GetWarningRichText() const { return WarningRichText; }
	void SetWarningRichText(const FText& Value) { WarningRichText = Value; InvalidateSearchableText(); }
#if !UE_BUILD_SHIPPING
	/** This version is for cheats and other non-shipping items, that don't need to localize their text.  We don't permit this in shipping to prevent unlocalized text being introduced. */
	/** 此版本适用于作弊道具及其他非发货商品，这类商品无需对文本进行本地化处理。在发货环节中我们不允许采用此方式，以避免引入未本地化的文本内容。*/
	void SetWarningRichText(const FString& Value) { SetWarningRichText(FText::FromString(Value)); }
#endif

	/**
	 * Gets the edit state of this property based on the current state of its edit conditions as well as any additional
	 * filter state.
	 *
	 *	根据该属性的编辑条件当前状态以及任何附加的筛选状态，获取该属性的编辑状态。
	 */
	const FGameSettingEditableState& GetEditState() const { return EditableStateCache; }

	/** Adds a new edit condition to this setting, allowing you to control the visibility and edit-ability of this setting. */
	/** 为该设置添加一个新的编辑条件，使您能够控制此设置的可见性和可编辑性。*/
	UE_API void AddEditCondition(const TSharedRef<FGameSettingEditCondition>& InEditCondition);

	/** Add setting dependency, if these settings change, we'll re-evaluate edit conditions for this setting. */
	/** 添加设置依赖项，如果这些设置发生变化，我们将重新评估此设置的编辑条件。*/
	UE_API void AddEditDependency(UGameSetting* DependencySetting);

	/** The parent object that owns the setting, in most cases the collection, but for top level settings the registry. */
	/** 拥有该设置的父对象，在大多数情况下为集合，但对于顶级设置而言则是注册表。*/
	UE_API void SetSettingParent(UGameSetting* InSettingParent);
	UGameSetting* GetSettingParent() const { return SettingParent; }

	/** Should this setting be reported to analytics. */
	/** 是否应将此设置报告给分析系统。*/
	bool GetIsReportedToAnalytics() const { return bReportAnalytics; }
	void SetIsReportedToAnalytics(bool bReport) { bReportAnalytics = bReport; }

	/** Gets the analytics value for this setting. */
	/** 获取此设置的分析值。*/
	virtual FString GetAnalyticsValue() const { return TEXT(""); }

	/**
	 * Some settings may take an async amount of time to finish initializing.  The settings system will wait
	 * for all settings to be ready before showing the setting.
	 *
	 * 有些设置的初始化过程可能会耗费相当长的时间。设置系统会一直等待所有设置都准备就绪后，才会显示这些设置。
	 * 
	 */
	bool IsReady() const { return bReady; }

	/**
	 * Any setting can have children, this is so we can allow for the possibility of "collections" or "actions" that
	 * are not directly visible to the user, but are set by some means and need to have initial and restored values.
	 * In that case, you would likely have internal settings inside an action subclass that is set on another screen,
	 * but never directly listed on the settings panel.
	 *
	 * 任何设置都可以有子项，这是因为这样我们就能允许存在“集合”或“操作”这类对用户来说并不直接可见，但却是通过某种方式设定，并且需要有初始值和恢复值的情况。
	 * 在这种情况下，您可能会在某个操作子类中设置内部设置，这些设置是在另一屏幕中设定的，但不会直接列在设置面板中。
	 * 
	 */
	virtual TArray<UGameSetting*> GetChildSettings() { return TArray<UGameSetting*>(); }

	/**
	 * Refresh the editable state of the setting and notify that the state has changed so that any UI currently
	 * examining this setting is updated with the new options, or whatever.
	 */
	/**
	 * 重置设置的可编辑状态，并通知用户该状态已发生改变，以便当前正在检查此设置的任何用户界面能够根据新的选项等进行更新。
	 * 
	 */
	UE_API void RefreshEditableState(bool bNotifyEditConditionsChanged = true);

	/**
	 * We expect settings to change the live value immediately, but occasionally there are special settings
	 * that go are immediately stored to a temporary location but we don't actually apply them until later
	 * like selecting a new resolution.
	 *
	 * 我们期望设置能够立即更改实时值，但有时也会有一些特殊的设置，它们会立即被存储到临时位置，但直到稍后我们才会实际应用它们，比如选择新的分辨率。
	 * 
	 */
	UE_API void Apply();

	/** Gets the current world of the local player that owns these settings. */
	/** 获取当前属于拥有这些设置的本地玩家的世界。*/
	UE_API virtual UWorld* GetWorld() const override;

protected:
	/** 计划只执行一次的拓展启动接口 由UGameSetting::Initialize外部调用*/
	UE_API virtual void Startup();
	UE_API void StartupComplete();

	UE_API virtual void OnInitialized();
	UE_API virtual void OnApply();
	UE_API virtual void OnGatherEditState(FGameSettingEditableState& InOutEditState) const;
	UE_API virtual void OnDependencyChanged();

	/**  */
	UE_API virtual FText GetDynamicDetailsInternal() const;

	/** */
	UE_API void HandleEditDependencyChanged(UGameSetting* DependencySetting, EGameSettingChangeReason Reason);
	UE_API void HandleEditDependencyChanged(UGameSetting* DependencySetting);

	/** Regenerates the plain searchable text if it has been dirtied. */
	/** 如果原始可搜索文本已受损，则重新生成该文本。*/
	UE_API void RefreshPlainText() const;
	void InvalidateSearchableText() { bRefreshPlainSearchableText = true; }

	/** Notify that the setting changed */
	/** 发出设置已更改的通知 */
	UE_API void NotifySettingChanged(EGameSettingChangeReason Reason);
	UE_API virtual void OnSettingChanged(EGameSettingChangeReason Reason);

	/** Notify that the settings edit conditions changed.  This may mean it's now invisible, or disabled, or possibly that the options have changed in some meaningful way. */
	/** 通知设置编辑条件已发生变更。这可能意味着该设置现在不可见、已禁用，或者可能是其选项已发生了某种有意义的变化。*/
	UE_API void NotifyEditConditionsChanged();
	UE_API virtual void OnEditConditionsChanged();

	/** 计算可编辑状态 */
	UE_API FGameSettingEditableState ComputeEditableState() const;

protected:

	UPROPERTY(Transient)
	TObjectPtr<ULocalPlayer> LocalPlayer;

	UPROPERTY(Transient)
	TObjectPtr<UGameSetting> SettingParent;

	UPROPERTY(Transient)
	TObjectPtr<UGameSettingRegistry> OwningRegistry;

	FName DevName;
	FText DisplayName;
	ESlateVisibility DisplayNameVisibility = ESlateVisibility::SelfHitTestInvisible;
	FText DescriptionRichText;
	FText WarningRichText;

	/** A collection of tags for the settings.  These can just be arbitrary flags used by the UI to do different things. */
	/** 一组用于设置的标签。这些标签仅仅是用户界面用来执行不同操作的任意标志而已。*/
	FGameplayTagContainer Tags;

	FGetGameSettingsDetails DynamicDetails;

	/** Any edit conditions for this setting. */
	/** 此设置的任何编辑条件。*/
	TArray<TSharedRef<FGameSettingEditCondition>> EditConditions;

	// 目前没用到
	class FStringCultureCache
	{
		FStringCultureCache(TFunction<FString()> InStringGetter);

		void Invalidate();

		FString Get() const;

	private:
		mutable FString StringCache;
		mutable FCultureRef Culture;
		TFunction<FString()> StringGetter;
	};

	/** When the text changes, we invalidate the searchable text. */
	/** 当文本发生更改时，我们将清除可搜索文本的缓存。*/
	mutable bool bRefreshPlainSearchableText = true;
	/** When we set the rich text for a setting, we automatically generate the plain text. */
	/** 当我们为某个设置设置富文本格式时，系统会自动生成相应的纯文本内容。*/
	mutable FString AutoGenerated_DescriptionPlainText;

	/** Report as part of analytics, by default no setting reports, except GameSettingValues. */
	/** 作为分析的一部分生成报告，默认情况下不会生成任何设置报告，但会生成“游戏设置值”报告。*/
	bool bReportAnalytics = false;

private:

	/** Most settings are immediately ready, but some may require startup time before it's safe to call their functions. */
	/** 大多数设置在启动时即可生效，但有些设置可能需要一段时间的初始化过程，只有在确认可以调用其相关函数时才可进行操作。*/
	bool bReady = false;

	/** Prevent re-entrancy problems when announcing a setting has changed. */
	/** 防止在宣布设置已更改时出现重复进入的问题。*/
	bool bOnSettingChangedEventGuard = false;

	/** Prevent re-entrancy problems when announcing a setting has changed edit conditions. */
	/** 防止在宣布设置已发生变化时出现重复进入的问题（即防止出现编辑条件重复生效的情况）。*/
	bool bOnEditConditionsChangedEventGuard = false;

	/**  */
	bool bAdjustListViewPostRefresh = true;

	/** We cache the editable state of a setting when it changes rather than reprocessing it any time it's needed.  */
	/** 我们会在设置的值发生变化时对其进行缓存，而不是每次需要使用该值时都重新处理它。*/
	FGameSettingEditableState EditableStateCache;
};

#undef UE_API
