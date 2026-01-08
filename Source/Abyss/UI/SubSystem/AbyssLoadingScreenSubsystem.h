// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AbyssLoadingScreenSubsystem.generated.h"



class UObject;
class UUserWidget;
struct FFrame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadingScreenWidgetChangedDelegate, TSubclassOf<UUserWidget>, NewWidgetClass);

/**
 * Tracks/stores the current loading screen configuration in a place
 * that persists across map transitions
 */
/**
 * 将当前的加载界面配置信息存储在一个位置
 * 该位置能够在地图切换过程中保持不变
 * 
 */
UCLASS()
class ABYSS_API UAbyssLoadingScreenSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	
	UAbyssLoadingScreenSubsystem();

	// Sets the loading screen widget class to display inside the loading screen widget host
	// 将加载屏幕组件类设置为在加载屏幕组件容器内显示的样式
	UFUNCTION(BlueprintCallable)
	void SetLoadingScreenContentWidget(TSubclassOf<UUserWidget> NewWidgetClass);

	// Returns the last set loading screen widget class to display inside the loading screen widget host
	// 返回在加载屏幕控件容器中要显示的最后一个加载屏幕控件类
	UFUNCTION(BlueprintPure)
	TSubclassOf<UUserWidget> GetLoadingScreenContentWidget() const;


private:
	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess))
	FLoadingScreenWidgetChangedDelegate OnLoadingScreenWidgetChanged;

	UPROPERTY()
	TSubclassOf<UUserWidget> LoadingScreenWidgetClass;
};
