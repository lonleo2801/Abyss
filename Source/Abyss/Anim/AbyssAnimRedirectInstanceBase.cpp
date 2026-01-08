// Copyright (c) 2025 Leon Lee


#include "AbyssAnimRedirectInstanceBase.h"
#include "Retargeter/IKRetargeter.h"

UAbyssAnimRedirectInstanceBase::UAbyssAnimRedirectInstanceBase()
{
}

void UAbyssAnimRedirectInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	UpdateChainSettings();
}

void UAbyssAnimRedirectInstanceBase::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	UpdateChainSettings();
}

FRetargetProfile UAbyssAnimRedirectInstanceBase::GetRetargetProfile() const
{
	
	FTargetChainSettings CpyLeftChainSettings = ChainSettings_LeftArm;

	float LeftAlpha = GetCurveValue(TEXT("Retarget_Hand_L_BlendToSource"));
	CpyLeftChainSettings.IK.BlendToSource = FMath::Lerp(ChainSettings_LeftArm.IK.BlendToSource,1.f, LeftAlpha);
	CpyLeftChainSettings.IK.StaticLocalOffset = FMath::Lerp(ChainSettings_LeftArm.IK.StaticLocalOffset, FVector::ZeroVector, LeftAlpha);

	FTargetChainSettings CpyRightChainSettings = ChainSettings_RightArm;

	float RightAlpha = GetCurveValue(TEXT("Retarget_Hand_R_BlendToSource"));
	CpyRightChainSettings.IK.BlendToSource = FMath::Lerp(ChainSettings_RightArm.IK.BlendToSource, 1.f, RightAlpha);
	CpyRightChainSettings.IK.StaticLocalOffset = FMath::Lerp(ChainSettings_RightArm.IK.StaticLocalOffset, FVector::ZeroVector, RightAlpha);

	FRetargetProfile InTargetProfile;
	InTargetProfile.ChainSettings.Add(TEXT("LeftArm"), CpyLeftChainSettings);
	InTargetProfile.ChainSettings.Add(TEXT("RightArm"), CpyRightChainSettings);

	return InTargetProfile;
}

void UAbyssAnimRedirectInstanceBase::UpdateChainSettings()
{
	if (GetOwningComponent())
	{
		if (!GetOwningComponent()->ComponentTags.IsEmpty())
		{
			FName InFindTag = GetOwningComponent()->ComponentTags[0];

			if (UIKRetargeter** InRetargeter = IKRetargeter_Map.Find(InFindTag))
			{
				IKRetargeter = *InRetargeter;

				ChainSettings_LeftArm = UIKRetargeter::GetChainSettingsFromRetargetAsset(*InRetargeter, TEXT("LeftArm"), NAME_None);
				ChainSettings_RightArm = UIKRetargeter::GetChainSettingsFromRetargetAsset(*InRetargeter, TEXT("RightArm"), NAME_None);
			}
		}
	}
}
