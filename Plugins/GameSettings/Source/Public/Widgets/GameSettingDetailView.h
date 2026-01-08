// Copyright Epic Games, Inc. All Rights Reserved.
// Finished.
#pragma once

#include "Blueprint/UserWidgetPool.h"

#include "GameSettingDetailView.generated.h"

#define UE_API GAMESETTINGS_API

class UCommonRichTextBlock;
class UCommonTextBlock;
class UGameSetting;
class UGameSettingDetailExtension;
class UGameSettingVisualData;
class UObject;
class UVerticalBox;
struct FStreamableHandle;

/**
 * 游戏设置细节面板
 */
UCLASS(MinimalAPI, Abstract)
class UGameSettingDetailView : public UUserWidget
{
	GENERATED_BODY()
public:
	// 构造函数 用来指定用户控件池
	UE_API UGameSettingDetailView(const FObjectInitializer& ObjectInitializer);

	// 根据游戏设置来填充细节面板
	UE_API void FillSettingDetails(UGameSetting* InSetting);

	//UVisual interface
	// 释放用户控件池子里面的资源
	UE_API virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

protected:
	// 无
	UE_API virtual void NativeConstruct() override;
	// 初始化一下空的面板
	UE_API virtual void NativeOnInitialized() override;
	// 根据游戏设置的控件类 创建它 并添加到竖向框中
	UE_API void CreateDetailsExtension(UGameSetting* InSetting, TSubclassOf<UGameSettingDetailExtension> ExtensionClass);

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UGameSettingVisualData> VisualData;
	/**
	 * FUserWidgetPool
	 * 	将 UUserWidget 实例进行池化处理，以减少 UMG 元素（具有动态项）的 UObject 和 SWidget 分配量。*
	 * 请注意，如果在用户界面组件（UserWidget）变为非活动状态时，其底层的 Slate 实例被释放，那么当 UUserWidget 组件分别变为活动或非活动状态时，NativeConstruct 和 NativeDestruct 方法将会被调用。前提是该组件在 Slate 层次结构中没有被实际引用（即如果该组件的共享引用计数从非零变为零或从零变为非零）。*
	 * 警告：务必在所属控件的“释放 Slate 资源”方法中释放池中的“Slate 组件”，以防止因循环引用而出现资源泄漏
	 * 否则，对 SObjectWidget 的缓存引用将使 UUserWidget（以及它们所引用的所有内容）保持处于活动状态*
	 * @参见 UListView
	 * @参见 UDynamicEntryBox
	 * 
	 */
	UPROPERTY(Transient)
	FUserWidgetPool ExtensionWidgetPool;

	// 目前使用的游戏设置
	UPROPERTY(Transient)
	TObjectPtr<UGameSetting> CurrentSetting;

	// 异步加载得句柄 因为有些控件类使用时可能还没有加载
	TSharedPtr<FStreamableHandle> StreamingHandle;

private:
	// Bound Widgets
	// 绑定得控件
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UCommonTextBlock> Text_SettingName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UCommonRichTextBlock> RichText_Description;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UCommonRichTextBlock> RichText_DynamicDetails;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UCommonRichTextBlock> RichText_WarningDetails;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UCommonRichTextBlock> RichText_DisabledDetails;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UVerticalBox> Box_DetailsExtension;
};

#undef UE_API
