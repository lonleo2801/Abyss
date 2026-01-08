// Copyright (c) 2025 Leon Lee


#include "TargetDataUnderCrosshair.h"

#include "AbilitySystemComponent.h"
#include "Abyss.h"

UTargetDataUnderCrosshair* UTargetDataUnderCrosshair::CreateTargetDataUnderCrosshair(UGameplayAbility* OwningAbility,bool bRepeat, float Interval)
{
	UTargetDataUnderCrosshair* MyObj = NewAbilityTask<UTargetDataUnderCrosshair>(OwningAbility);
	MyObj->bShouldRepeat = bRepeat;
	MyObj->ReportInterval = Interval;
	return MyObj;
}

void UTargetDataUnderCrosshair::Activate()
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
    
	if (bIsLocallyControlled)
	{
		// 第一次立即执行
		SendCrosshairTargetData();

		// 如果需要重复，开启定时器
		if (bShouldRepeat)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_Trace, this, &UTargetDataUnderCrosshair::SendCrosshairTargetData, ReportInterval, true);
		}
	}
	else // Server
	{
		// 服务器端监听客户端上传的数据
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
        
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UTargetDataUnderCrosshair::OnTargetDataReplicatedCallback);
        
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
        
		if (!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderCrosshair::OnDestroy(bool bInOwnerFinished)
{
	if (Ability)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Trace);
	}
	Super::OnDestroy(bInOwnerFinished);
}

void UTargetDataUnderCrosshair::SendCrosshairTargetData()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());


	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	// 获取视窗大小
	int32 SizeX, SizeY;
	PC->GetViewportSize(SizeX, SizeY);

	// 准星位置（正中心）
	const float ScreenX = SizeX * 0.5f;
	const float ScreenY = SizeY * 0.5f;

	FVector WorldLocation, WorldDirection;

	// 将屏幕坐标反投影到世界方向
	PC->DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldLocation, WorldDirection);
	
	const FVector Start = WorldLocation;
	const FVector End = Start + WorldDirection * 8000.f; // 你的最大瞄准距离

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Ability->GetCurrentActorInfo()->AvatarActor.Get());

	FGameplayAbilityTargetDataHandle DataHandle;
	// 使用你的敌人碰撞通道
	if (GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_CrosshairTrace, // 攻击敌人用的碰撞通道
		Params
	))
	{
		FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
		Data->HitResult = Hit;
		DataHandle.Add(Data);
	}

	// ... 构建 DataHandle ...
	// 发送数据给服务器
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
	
}

void UTargetDataUnderCrosshair::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
