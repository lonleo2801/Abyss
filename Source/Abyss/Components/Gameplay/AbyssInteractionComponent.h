// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Components/AbyssExtensionComponentBase.h"
#include "AbyssInteractionComponent.generated.h"

struct FInputActionValue;
class UPickupPromptWidget;
struct FInteractionData;
class IInteractableInterface;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ABYSS_API UAbyssInteractionComponent : public UAbyssExtensionComponentBase
{
	GENERATED_BODY()

public:
	UAbyssInteractionComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
	
	// 玩家按下 F 键调用此函数
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryInteract();

protected:
	
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Abyss|TraceConfig")
	float TraceDistance = 450.0f;

	// 当前聚焦的物体
	UPROPERTY(VisibleAnywhere)
	TScriptInterface<IInteractableInterface> CurrentInteractable;

	// 执行射线检测
	void PerformInteractionCheck();

	// RPC: 通知服务器执行交互
	UFUNCTION(Server, Reliable)
	void ServerTriggerInteraction(UObject* TargetObject);
	
	UPROPERTY(EditAnywhere,Category="Abyss|Pickup")
	TSubclassOf<UPickupPromptWidget> PickupWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = "Abyss|Input")
	class UInputAction* InteractAction;
	
	// 输入回调函数
	void Input_Interact_Started(const FInputActionValue& Value);
	void Input_Interact_Completed(const FInputActionValue& Value);
	
private:
	void OnFoundInteractable(const FInteractionData& Data);
	
	void OnLostInteractable();
	
	// 这里保存的是实际生成的 UI 实例 (弱指针，防止内存泄漏或野指针)
	TWeakObjectPtr<UPickupPromptWidget> ActivePickupWidget;
};

