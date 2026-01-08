// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Retargeter/IKRetargetDeprecated.h"
#include "Retargeter/IKRetargetProfile.h"
#include "AbyssAnimRedirectInstanceBase.generated.h"

class UIKRetargeter;
/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssAnimRedirectInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

public:
	UAbyssAnimRedirectInstanceBase();

public:
	virtual void NativeInitializeAnimation() override;
	//virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	TObjectPtr<UIKRetargeter> IKRetargeter;

	UPROPERTY(EditDefaultsOnly, Category = "AnimAttrubute")
	TMap<FName,UIKRetargeter*> IKRetargeter_Map;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	FTargetChainSettings ChainSettings_LeftArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	FTargetChainSettings ChainSettings_RightArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimAttrubute")
	FRetargetProfile CustomRetargetProfile;

public:
	UFUNCTION(BlueprintPure, Category = "RedirectAnim", meta = (BlueprintThreadSafe))
	FRetargetProfile GetRetargetProfile() const;

protected:
	void UpdateChainSettings();
};
