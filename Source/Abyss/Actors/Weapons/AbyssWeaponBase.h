// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Actors/AbyssPickupActor.h"
#include "AbyssWeaponBase.generated.h"

class UWeaponDefinition;
// 定义武器状态
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	OnGround    UMETA(DisplayName = "On Ground (Physics)"),
	Equipped    UMETA(DisplayName = "Equipped (In Hand)"),
	Holstered   UMETA(DisplayName = "Holstered (On Back/Hip)")
};
UCLASS()
class ABYSS_API AAbyssWeaponBase : public AAbyssPickupActor
{
	GENERATED_BODY()

public:	
	AAbyssWeaponBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// --- 数据 ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abyss|Setup")
	UWeaponDefinition* WeaponDef;

	// --- 状态管理 (核心) ---
	// 服务器调用此函数来改变状态
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "State")
	void SetWeaponState(EWeaponState NewState);

	// 获取当前状态
	UFUNCTION(BlueprintPure, Category = "State")
	EWeaponState GetWeaponState() const { return CurrentState; }

	// --- 接口实现 ---
	virtual bool TriggerInteraction_Implementation(APawn* Interactor) override;


protected:
	virtual void BeginPlay() override;

	// 当前状态 (Replicated)
	UPROPERTY(ReplicatedUsing = OnRep_CurrentState, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EWeaponState CurrentState;

	// 状态回调：处理视觉、挂载、物理开关
	UFUNCTION()
	void OnRep_CurrentState();

	// 真正的逻辑执行函数
	void UpdateAttachmentLogic();
};
