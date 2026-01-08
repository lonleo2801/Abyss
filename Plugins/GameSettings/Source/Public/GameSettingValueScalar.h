// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameSettingValue.h"
#include "Math/Range.h"

#include "GameSettingValueScalar.generated.h"

#define UE_API GAMESETTINGS_API

class UObject;

// 标量数值
UCLASS(MinimalAPI, abstract)
class UGameSettingValueScalar : public UGameSettingValue
{
	GENERATED_BODY()

public:
	// 构造函数 无
	UE_API UGameSettingValueScalar();
	// 传入归一化的值 然后根据范围 得到实际的值
	UE_API void SetValueNormalized(double NormalizedValue);

	// 获取当前归一化的值
	UE_API double GetValueNormalized() const;
	// 获取默认的归一化值
	TOptional<double> GetDefaultValueNormalized() const
	{
		TOptional<double> DefaultValue = GetDefaultValue();
		if (DefaultValue.IsSet())
		{
			return FMath::GetMappedRangeValueClamped(GetSourceRange(), TRange<double>(0, 1), DefaultValue.GetValue());
		}
		return TOptional<double>();
	}
	// 获取默认的值
	UE_API virtual TOptional<double> GetDefaultValue() const						PURE_VIRTUAL(, return TOptional<double>(););
	// 设置具体的值
	UE_API virtual void SetValue(double Value, EGameSettingChangeReason Reason = EGameSettingChangeReason::Change)	PURE_VIRTUAL(, );
	// 获取具体的值
	UE_API virtual double GetValue() const											PURE_VIRTUAL(, return 0;);
	/**
	* 范围模板。*
	* 注意：此类别不适用于区间运算（有关区间运算的相关内容请参阅 TInterval 类）。*
	* 一个范围表示的是一个连续的元素集合，它仅存储该集合的下限值和上限值（即端点值），以提高存储效率。必然的
	* 值可以是排他性的（该值不属于范围之内）、包含性的（该值属于范围之内）或者开放式的（没有对值的限制）。*
	该模板的主要关注点在于连续范围，但也可用于表示离散范围。离散范围的元素类型具有明确的步长，例如整数或日期，它将相邻的元素分隔开来。这与连续范围不同，在连续范围内，步长（如浮点数或时间跨度）并不重要，而且在任意两个元素之间可能会存在其他元素（尽管实际上由于计算机数值的有限精度，所有范围都是离散的）。
	* 在使用范围时，此模板的使用者需确保正确理解范围的起始和结束点。
	* 不同的语义会因范围是在连续域还是离散域中被解读而有所不同。*
	* 离散范围 [A, B) 的迭代包括从 A 到 B - 1 的所有元素。连续范围内的元素通常不进行迭代。同时，还需要考虑离散范围不同表示形式之间的等价性。对于
	例如，在离散域中，区间 [2, 6)、(1, 5] 和 [2, 5] 是等价的，
	但在连续域中则不同。为了使这个类保持简单，我们没有包含规范化函数或辅助模板参数，比如单位和最小/最大域元素。为了在大多数常见用例中便于使用，建议将所有操作限制在形式为 [A， B) 的规范区间内，其中下限包含在内，上限不包含在区间内。*
	* @参数 ElementType：表示该范围所代表元素的类型
	* @参考 RangeBound、TInterval*/
	// 映射的范围
	UE_API virtual TRange<double> GetSourceRange() const							PURE_VIRTUAL(, return TRange<double>(););
	// 步进的值
	UE_API virtual double GetSourceStep() const									PURE_VIRTUAL(, return 0.01;);
	// 获取归一化的每次步进的值
	double GetNormalizedStepSize() const
	{
		TRange<double> SourceRange = GetSourceRange();
		return GetSourceStep() / FMath::Abs(SourceRange.GetUpperBoundValue() - SourceRange.GetLowerBoundValue());
	}
	// 获取格式化后的文本
	UE_API virtual FText GetFormattedText() const									PURE_VIRTUAL(, return FText::GetEmpty(););
	// 获取用于分析的当前值
	virtual FString GetAnalyticsValue() const override
	{
		return LexToString(GetValue());
	}

protected:
};

#undef UE_API
