// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/CancellableAsyncAction.h"
#include "GameplayTagContainer.h"
#include "UObject/SoftObjectPtr.h"

#include "AsyncAction_PushContentToLayerForPlayer.generated.h"

#define UE_API COMMONGAME_API

class APlayerController;
class UCommonActivatableWidget;
class UObject;
struct FFrame;
struct FStreamableHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPushContentToLayerForPlayerAsyncDelegate, UCommonActivatableWidget*, UserWidget);

/**
 * 推送控件到根布局!
 */
UCLASS(MinimalAPI, BlueprintType)
class UAsyncAction_PushContentToLayerForPlayer : public UCancellableAsyncAction
{
	GENERATED_UCLASS_BODY()

public:
	UE_API virtual void Cancel() override;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta=(WorldContext = "WorldContextObject", BlueprintInternalUseOnly="true"))
	static UE_API UAsyncAction_PushContentToLayerForPlayer* PushContentToLayerForPlayer(APlayerController* OwningPlayer, UPARAM(meta = (AllowAbstract=false)) TSoftClassPtr<UCommonActivatableWidget> WidgetClass, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, bool bSuspendInputUntilComplete = true);

	// 开始异步行动 由蓝图自行调用 C++需要手动!
	UE_API virtual void Activate() override;

public:
	// 推送前执行的代理 注意 这个时候根据容器的执行逻辑 还没激活 只是实例化了
	UPROPERTY(BlueprintAssignable)
	FPushContentToLayerForPlayerAsyncDelegate BeforePush;
	// 推送后执行的代理 注意 这个时候根据容器的执行逻辑 已经激活 
	UPROPERTY(BlueprintAssignable)
	FPushContentToLayerForPlayerAsyncDelegate AfterPush;

private:

	// 图层名字
	FGameplayTag LayerName;

	// 是否中断输入
	bool bSuspendInputUntilComplete = false;

	// 玩家控制器
	TWeakObjectPtr<APlayerController> OwningPlayerPtr;

	// 推送的控件类的软引用
	TSoftClassPtr<UCommonActivatableWidget> WidgetClass;

	// 资产加载句柄
	TSharedPtr<FStreamableHandle> StreamingHandle;
};

#undef UE_API
