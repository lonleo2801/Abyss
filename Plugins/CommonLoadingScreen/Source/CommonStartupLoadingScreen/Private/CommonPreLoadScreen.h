// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PreLoadScreenBase.h"

class SWidget;
// 基类
// 用于处理预加载屏幕相关逻辑的基类实现，负责控制/更新预加载屏幕的用户界面。
// 设计为可被特定游戏插件重写，该插件会调用 FPreloadScreenManager:：RegisterPreLoadScreen 方法，以便预加载屏幕管理器能够正确调用相关函数。

class FCommonPreLoadScreen : public FPreLoadScreenBase
{
public:
	
    /*** IPreLoadScreen Implementation ***/
	// 创建控件
	virtual void Init() override;
	// 指定显示的阶段
    virtual EPreLoadScreenTypes GetPreLoadScreenType() const override { return EPreLoadScreenTypes::EngineLoadingScreen; }
	// 返回给引擎用以使用
    virtual TSharedPtr<SWidget> GetWidget() override { return EngineLoadingWidget; }

private:

    TSharedPtr<SWidget> EngineLoadingWidget;
};
