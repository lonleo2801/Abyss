// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameSettingFilterState.h"
#include "GameSetting.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameSettingFilterState)

#define LOCTEXT_NAMESPACE "GameSetting"

class FSettingFilterExpressionContext : public ITextFilterExpressionContext
{
public:
	explicit FSettingFilterExpressionContext(const UGameSetting& InSetting) : Setting(InSetting) {}

	virtual bool TestBasicStringExpression(const FTextFilterString& InValue, const ETextFilterTextComparisonMode InTextComparisonMode) const override
	{
		return TextFilterUtils::TestBasicStringExpression(Setting.GetDescriptionPlainText(), InValue, InTextComparisonMode);
	}

	virtual bool TestComplexExpression(const FName& InKey, const FTextFilterString& InValue, const ETextFilterComparisonOperation InComparisonOperation, const ETextFilterTextComparisonMode InTextComparisonMode) const override
	{
		return false;
	}

private:
	/** Setting being filtered. */
	const UGameSetting& Setting;
};

//--------------------------------------
// FGameSettingFilterState
//--------------------------------------

FGameSettingFilterState::FGameSettingFilterState()
	: SearchTextEvaluator(ETextFilterExpressionEvaluatorMode::BasicString)
{
}

void FGameSettingFilterState::AddSettingToRootList(UGameSetting* InSetting)
{
	SettingAllowList.Add(InSetting);
	SettingRootList.Add(InSetting);
}

void FGameSettingFilterState::AddSettingToAllowList(UGameSetting* InSetting)
{
	SettingAllowList.Add(InSetting);
}

void FGameSettingFilterState::SetSearchText(const FString& InSearchText)
{
	SearchTextEvaluator.SetFilterText(FText::FromString(InSearchText));
}

bool FGameSettingFilterState::DoesSettingPassFilter(const UGameSetting& InSetting) const
{
	const FGameSettingEditableState& EditableState = InSetting.GetEditState();

	// 如果不包含隐藏的 并且本身状态时隐藏的 则不通过
	if (!bIncludeHidden && !EditableState.IsVisible())
	{
		return false;
	}
	// 如果不包含关闭的 并且本身状态时关闭的 则不通过
	if (!bIncludeDisabled && !EditableState.IsEnabled())
	{
		return false;
	}
	// 这里有点绕
	// 如果不包含可以恢复默认 并且本身是 不可以恢复至默认的 则不通过

	// 当前设置的状态是可以恢复到默认 直接通过
	// 当前要求的状态是包含可以恢复到默认的 直接通过
	// 当前要求的状态是不包含可以恢复到默认的 游戏设置的状态是可以恢复到默认的 直接通过
	// 当前要求的状态是不包含可以恢复到默认的 游戏设置的状态是不可以恢复到默认的 判定不通过.->只屏蔽不可重置的游戏设置.
	if (!bIncludeResetable && !EditableState.IsResetable())
	{
		return false;
	}

	// Are we filtering settings?
	// 我们是在筛选设置吗？
	if (SettingAllowList.Num() > 0)
	{
		if (!SettingAllowList.Contains(&InSetting))
		{
			bool bAllowed = false;
			const UGameSetting* NextSetting = &InSetting;
			while (const UGameSetting* Parent = NextSetting->GetSettingParent())
			{
				if (SettingAllowList.Contains(Parent))
				{
					bAllowed = true;
					break;
				}

				NextSetting = Parent;
			}

			if (!bAllowed)
			{
				return false;
			}
		}
	}

	// TODO more filters...

	// Always search text last, it's generally the most expensive filter.
	// 总是最后再搜索文本内容，因为这通常是最昂贵的筛选方式。
	if (!SearchTextEvaluator.TestTextFilter(FSettingFilterExpressionContext(InSetting)))
	{
		return false;
	}

	return true;
}

//--------------------------------------
// FGameSettingsEditableState
//--------------------------------------

void FGameSettingEditableState::Hide(const FString& DevReason)
{
#if !UE_BUILD_SHIPPING
	ensureAlwaysMsgf(!DevReason.IsEmpty(), TEXT("To hide a setting, you must provide a developer reason."));
#endif

	bVisible = false;

#if !UE_BUILD_SHIPPING
	HiddenReasons.Add(DevReason);
#endif
}

void FGameSettingEditableState::Disable(const FText& Reason)
{
#if !UE_BUILD_SHIPPING
	ensureAlwaysMsgf(!Reason.IsEmpty(), TEXT("To disable a setting, you must provide a reason that we can show players."));
#endif

	bEnabled = false;
	DisabledReasons.Add(Reason);
}

void FGameSettingEditableState::DisableOption(const FString& Option)
{
#if !UE_BUILD_SHIPPING
	ensureAlwaysMsgf(!DisabledOptions.Contains(Option), TEXT("You've already disabled this option."));
#endif

	DisabledOptions.Add(Option);
}

void FGameSettingEditableState::UnableToReset()
{
	bResetable = false;
}

#undef LOCTEXT_NAMESPACE

