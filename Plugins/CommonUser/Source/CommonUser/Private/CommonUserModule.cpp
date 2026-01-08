// Copyright Epic Games, Inc. All Rights Reserved.

#include "CommonUserModule.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FCommonUserModule"

void FCommonUserModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	// 此代码将在您的模块被加载至内存后执行；具体执行时间会在每个模块的 .uplugin 文件中进行详细说明。
}

void FCommonUserModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	// 在系统关闭时，此函数可用于清理您的模块。对于支持动态重新加载的模块，我们会在卸载模块之前调用此函数。
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCommonUserModule, CommonUser)