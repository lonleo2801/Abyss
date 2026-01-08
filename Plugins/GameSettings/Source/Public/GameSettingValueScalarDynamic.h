// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameSettingValueScalar.h"

#include "GameSettingValueScalarDynamic.generated.h"

#define UE_API GAMESETTINGS_API

struct FNumberFormattingOptions;

class FGameSettingDataSource;
class UObject;

//////////////////////////////////////////////////////////////////////////
// UGameSettingValueScalarDynamic
//////////////////////////////////////////////////////////////////////////

typedef TFunction<FText(double SourceValue, double NormalizedValue)> FSettingScalarFormatFunction;

UCLASS(MinimalAPI)
class UGameSettingValueScalarDynamic : public UGameSettingValueScalar
{
	GENERATED_BODY()

public:
	// 格式化数字
	static UE_API FSettingScalarFormatFunction Raw;
	static UE_API FSettingScalarFormatFunction RawOneDecimal;
	static UE_API FSettingScalarFormatFunction RawTwoDecimals;
	static UE_API FSettingScalarFormatFunction ZeroToOnePercent;
	static UE_API FSettingScalarFormatFunction ZeroToOnePercent_OneDecimal;
	static UE_API FSettingScalarFormatFunction SourceAsPercent1;
	static UE_API FSettingScalarFormatFunction SourceAsPercent100;
	static UE_API FSettingScalarFormatFunction SourceAsInteger;
private:
	static const FNumberFormattingOptions& GetOneDecimalFormattingOptions();
	
public:
	// 构造函数 无
	UE_API UGameSettingValueScalarDynamic();

	/** UGameSettingValue */
	// 一次性接口 不执行父类 主要是用于等待对应的数据源初始化好 然后才使得自己初始化完成 Ready
	UE_API virtual void Startup() override;
	// 存储当前值为初始值
	UE_API virtual void StoreInitial() override;
	// 恢复默认值
	UE_API virtual void ResetToDefault() override;
	// 恢复初始值
	UE_API virtual void RestoreToInitial() override;

	/** UGameSettingValueScalar */
	// 获取默认值
	UE_API virtual TOptional<double> GetDefaultValue() const override;
	// 设置当前值
	UE_API virtual void SetValue(double Value, EGameSettingChangeReason Reason = EGameSettingChangeReason::Change) override;
	// 获取当前值
	UE_API virtual double GetValue() const override;
	// 获取源数据的范围
	UE_API virtual TRange<double> GetSourceRange() const override;
	// 获取源数据的步长
	UE_API virtual double GetSourceStep() const override;
	// 获取格式化后的展示文本
	UE_API virtual FText GetFormattedText() const override;

	/** UGameSettingValueDiscreteDynamic */
	UE_API void SetDynamicGetter(const TSharedRef<FGameSettingDataSource>& InGetter);
	UE_API void SetDynamicSetter(const TSharedRef<FGameSettingDataSource>& InSetter);
	UE_API void SetDefaultValue(double InValue);

	/**  */
	// 设置显示的格式
	UE_API void SetDisplayFormat(FSettingScalarFormatFunction InDisplayFormat);
	
	/** 设置源数据变动范围和 步进 */
	UE_API void SetSourceRangeAndStep(const TRange<double>& InRange, double InSourceStep);
	
	/**
	 * The SetSourceRangeAndStep defines the actual range the numbers could move in, but often
	 * the true minimum for the user is greater than the minimum source range, so for example, the range
	 * of some slider might be 0..100, but you want to restrict the slider so that while it shows 
	 * a bar that travels from 0 to 100, the user can't set anything lower than some minimum, e.g. 1.
	 * That is the Minimum Limit.
	 */

	/**
	 * “SetSourceRangeAndStep”定义了数字能够移动的实际范围，但通常
	 * 用户实际所需的最小值会大于源范围的最小值，例如，某些滑块的范围可能是 0 至 100，但您希望对滑块进行限制，使得尽管它显示的条形从 0 移动到 100，但用户无法将数值设置得低于某个最小值，比如 1。
	 * 这就是“最小限制”。
	 * 
	 */
	UE_API void SetMinimumLimit(const TOptional<double>& InMinimum);

	/**
	 * 	“SetSourceRangeAndStep”定义了数字能够移动的实际范围，但通常情况下
	 * 用户实际所能达到的最大值会小于源数据的最大范围，例如，某些滑块的范围可能是 0 到 100，但您希望对滑块进行限制，使得尽管它显示的条形从 0 移动到 100，但用户无法将数值设置得低于某个最大值，比如 95。
	 * 这就是最大限制值。
	 * 
	 */
	UE_API void SetMaximumLimit(const TOptional<double>& InMaximum);
	
protected:
	/** UGameSettingValue */
	// 非空判定
	UE_API virtual void OnInitialized() override;

	// 数据源准备好了
	UE_API void OnDataSourcesReady();

protected:

	TSharedPtr<FGameSettingDataSource> Getter;
	TSharedPtr<FGameSettingDataSource> Setter;

	TOptional<double> DefaultValue;
	double InitialValue = 0;

	TRange<double> SourceRange = TRange<double>(0, 1);
	double SourceStep = 0.01;
	/**
	* 当我们有一个可选值时，IsSet() 函数会返回真，并且 GetValue() 函数才有意义。
	* 反之，GetValue() 函数则没有意义。*/
	TOptional<double> Minimum;
	TOptional<double> Maximum;

	FSettingScalarFormatFunction DisplayFormat;
};

#undef UE_API
