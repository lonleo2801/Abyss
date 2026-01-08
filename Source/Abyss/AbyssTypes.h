// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AbyssTypes.generated.h"


UENUM()
enum class EAbyssValidType :uint8
{
	Valid,
	Invalid
};

/**
 * 
 */
//描述角色的旋转步态
UENUM(BlueprintType)
enum class ECharacterRotationModeType :uint8
{
	OrientToMovement		UMETA(DisplayName = "Orient To Movement"),
	Strafe					UMETA(DisplayName = "Strafe"),
};

//描述角色的步态
UENUM(BlueprintType)
enum class ECharacterGaitType :uint8
{
	Gait_Walk			UMETA(DisplayName = "Walk"),
	Gait_Run			UMETA(DisplayName = "Run"),
	Gait_Sprint			UMETA(DisplayName = "Sprint"),
};

//移动模式
UENUM(BlueprintType)
enum class ECharacterMovementModeType :uint8
{
	Movement_OnGround		UMETA(DisplayName = "OnGround"),
	Movement_InAir			UMETA(DisplayName = "InAir"),
};

//移动状态
UENUM(BlueprintType)
enum class ECharacterMovementStateType :uint8
{
	Movement_Idle			UMETA(DisplayName = "Idle"),
	Movement_Moving			UMETA(DisplayName = "Moving"),
	Movement_InAir			UMETA(DisplayName = "In Air"),
};

//站立姿态
UENUM(BlueprintType)
enum class ECharacterStandType :uint8
{
	Character_Stand			UMETA(DisplayName = "Stand"),
	Character_Crouch		UMETA(DisplayName = "Crouch"),
};



//Reset
struct FResetBool
{
	FResetBool()
		:bSet(false)
		, Time(0.f)
		, bNeedExecute(false)
	{
	}

	void Tick(float DeltaTime)
	{
		if (Time > 0.f)
		{
			Time -= DeltaTime;

			Update.ExecuteIfBound();

			if (Time <= 0.f)
			{
				Fun.ExecuteIfBound();
				Time = 0.f;
				bNeedExecute = false;
			}
		}
		else
		{
			if (bNeedExecute)//由于时间帧和帧间隔过大，导致fun没有执行
			{
				bNeedExecute = false;

				Update.ExecuteIfBound();//至少执行一次 更新
				Fun.ExecuteIfBound(); //至少执行一次 Func 更新

				Time = 0.f;
			}

			bSet = false;
		}
	}

	FResetBool& operator=(bool bNewSet)
	{
		if (bNewSet)
		{
			StartFun.ExecuteIfBound();
		}

		if (bNewSet &&
			Time > 0.f)
		{
			bNeedExecute = bNewSet;
		}

		InitFun.ExecuteIfBound();

		bSet = bNewSet;
		return *this;
	}

	FResetBool& operator=(float InNewTime)
	{
		//只要设置时间 就需要执行
		if (InNewTime > 0.f)
		{
			bNeedExecute = true;
		}

		Time = InNewTime;
		return *this;
	}

	bool operator*() const
	{
		return bSet;
	}

	explicit operator bool() const
	{
		return bSet;
	}

	float GetTime() const { return Time; }
	void ForceExecute() const { Fun.ExecuteIfBound(); }

	FSimpleDelegate StartFun;
	FSimpleDelegate InitFun;
	FSimpleDelegate Fun;
	FSimpleDelegate Update;

protected:
	bool bSet;
	float Time;

	//防止帧时间过大 而忽略执行 fun和update代理
	bool bNeedExecute;
};
