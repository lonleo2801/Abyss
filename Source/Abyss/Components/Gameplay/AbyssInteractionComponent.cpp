// Copyright (c) 2025 Leon Lee


#include "AbyssInteractionComponent.h"

#include "Abyss.h"
#include "CommonUIExtensions.h"
#include "EnhancedInputComponent.h"
#include "Components/Input/AbyssInputComponent.h"
#include "GameplayTags/AbyssUITags.h"
#include "Interfaces/InteractableInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/PromptWidgets/PickupPromptWidget.h"


UAbyssInteractionComponent::UAbyssInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
}


void UAbyssInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 只有本地控制的玩家才执行检测
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn && OwnerPawn->IsLocallyControlled())
	{
		PerformInteractionCheck();
	}
}

void UAbyssInteractionComponent::TryInteract()
{
	if (CurrentInteractable.GetObject())
	{
		ServerTriggerInteraction(CurrentInteractable.GetObject());
		OnLostInteractable();
	}
}

void UAbyssInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwningPawn()->IsLocallyControlled() && GetOwningPawn()->InputComponent)
	{
		if (UAbyssInputComponent* AbyssEnhancedInputComponent = Cast<UAbyssInputComponent>(GetOwningPawn()->InputComponent))
		{
			// 绑定 "按下" (Started)
			AbyssEnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ThisClass::Input_Interact_Started);
        
			// 绑定 "松开" (Completed) -> 用于处理长按中断
			AbyssEnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &ThisClass::Input_Interact_Completed);
		}
	}
}

void UAbyssInteractionComponent::Input_Interact_Started(const FInputActionValue& Value)
{
	if (ActivePickupWidget.IsValid())
	{
		ActivePickupWidget->StartInteraction();
	}
	
}

void UAbyssInteractionComponent::Input_Interact_Completed(const FInputActionValue& Value)
{
	if (ActivePickupWidget.IsValid())
	{
		ActivePickupWidget->StopInteraction();
	}
}

void UAbyssInteractionComponent::PerformInteractionCheck()
{
	APlayerController* PC = GetOwningController<APlayerController>();
	// 获取视窗大小
	int32 SizeX, SizeY;
	PC->GetViewportSize(SizeX, SizeY);

	// 准星位置（正中心）
	const float ScreenX = SizeX * 0.5f;
	const float ScreenY = SizeY * 0.5f;

	FVector ViewLocation, ViewDirection;

	// 将屏幕坐标反投影到世界方向
	PC->DeprojectScreenPositionToWorld(ScreenX, ScreenY, ViewLocation, ViewDirection);
	
	const FVector Start = ViewLocation;
	const FVector End = Start + ViewDirection * TraceDistance; 
	
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
    
	// 使用 Interaction 通道 
	bool bHit =GetWorld()->LineTraceSingleByChannel(HitResult,Start,End,ECC_Interaction,Params);
	
	//bool bHit =UKismetSystemLibrary::LineTraceSingle(GetWorld(),Start,End,ETraceTypeQuery::TraceTypeQuery3,false,TArray<AActor*>(), EDrawDebugTrace::ForDuration,HitResult,true);
	
	if (bHit && HitResult.GetActor() && HitResult.GetActor()->Implements<UInteractableInterface>())
	{
		if (HitResult.GetActor() != CurrentInteractable.GetObject())
		{
			CurrentInteractable.SetObject(HitResult.GetActor());
			CurrentInteractable.SetInterface(Cast<IInteractableInterface>(HitResult.GetActor()));
            
			// 通知 UI
			FInteractionData Data = IInteractableInterface::Execute_GetInteractionData(CurrentInteractable.GetObject());
			OnFoundInteractable(Data);
		}
	}
	else if (CurrentInteractable.GetObject())
	{
		CurrentInteractable = nullptr;
		OnLostInteractable();
	}
}

void UAbyssInteractionComponent::OnFoundInteractable(const FInteractionData& Data)
{
	// 先清理旧的 Widget（如果存在）
	if (ActivePickupWidget.IsValid())
	{
		ActivePickupWidget->OnLongPressComplete.Clear();
		ActivePickupWidget->OnShortPressTriggered.Clear();
		ActivePickupWidget->DeactivateWidget();
	}
	APlayerController* PC = GetOwningController<APlayerController>();
	
	ULocalPlayer* LocalPlayer = PC ? PC->GetLocalPlayer() : nullptr;

	if (LocalPlayer)
	{
		
		ActivePickupWidget = Cast<UPickupPromptWidget>(UCommonUIExtensions::PushContentToLayer_ForPlayer(
		   LocalPlayer,
		   AbyssTags_UI::Layer::Modal,
		   PickupWidget
		));
		ActivePickupWidget->SetupInteraction(Data);
		ActivePickupWidget->OnLongPressComplete.AddDynamic(this, &ThisClass::TryInteract);
		ActivePickupWidget->OnShortPressTriggered.AddDynamic(this, &ThisClass::TryInteract);
	}
}

void UAbyssInteractionComponent::OnLostInteractable()
{
	APlayerController* PC = GetOwningController<APlayerController>();
	ULocalPlayer* LocalPlayer = PC ? PC->GetLocalPlayer() : nullptr;
	if (LocalPlayer)
	{
		if (ActivePickupWidget.IsValid())
		{
			ActivePickupWidget->OnLongPressComplete.Clear();
			ActivePickupWidget->OnShortPressTriggered.Clear();
			ActivePickupWidget->DeactivateWidget();
			ActivePickupWidget.Reset();
		}
	}
}

void UAbyssInteractionComponent::ServerTriggerInteraction_Implementation(UObject* TargetObject)
{
	if (TargetObject && TargetObject->Implements<UInteractableInterface>())
	{
		IInteractableInterface::Execute_TriggerInteraction(TargetObject, Cast<APawn>(GetOwner()));
	}
}

