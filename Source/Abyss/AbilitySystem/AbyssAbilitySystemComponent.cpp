// Copyright (c) 2025 Leon Lee


#include "AbyssAbilitySystemComponent.h"

#include "Abilities/AbyssGameplayAbility.h"
#include "Attributes/AbyssAttributeSet.h"
#include "Characters/Hero/Character/AbyssHeroBase.h"
#include "GameplayTags/AbyssTags.h"

void UAbyssAbilitySystemComponent::AbilityActorInfoSet()
{
	/** Called on server whenever a GE is applied to self. This includes instant and duration based GEs. */
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&ThisClass::ClientEffectApplied);
}

FGameplayAbilitySpecHandle UAbyssAbilitySystemComponent::AddCharacterAbility(TSubclassOf<UGameplayAbility> AbilityClass,int32 Level)
{
	// 1. 安全检查：必须是服务器，且类有效
	if (!IsOwnerActorAuthoritative())
	{
		return FGameplayAbilitySpecHandle(); // 返回一个无效的空 Handle
	}

	if (!AbilityClass)
	{
		return FGameplayAbilitySpecHandle();
	}

	// 2. 构建 Spec
	FGameplayAbilitySpec AbilitySpec(AbilityClass, Level);

	// 3. 处理 Input Tag (从 CDO 读取配置)
	if (const UAbyssGameplayAbility* AbyssAbility = Cast<UAbyssGameplayAbility>(AbilitySpec.Ability))
	{
		if (AbyssAbility->StartupInputTag.IsValid())
		{
			// 将 Input Tag 添加到 Spec 的动态标签中
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbyssAbility->StartupInputTag);
		}
	}

	// 4. 赋予技能并返回 Handle
	// GiveAbility 会在内部处理 Spec 的注册并返回生成的 Handle
	return GiveAbility(AbilitySpec);
}

void UAbyssAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	if (!IsOwnerActorAuthoritative())
	{
		return;
	}
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		if (!AbilityClass) continue;
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1);
		if (const UAbyssGameplayAbility* AbyssAbility = Cast<UAbyssGameplayAbility>(AbilitySpec.Ability))
		{
			if (AbyssAbility->StartupInputTag.IsValid())
			{
				AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbyssAbility->StartupInputTag);
			}
			GiveAbility(AbilitySpec);
		}
	}
}

void UAbyssAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UAbyssAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				//InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed,AbilitySpec.Handle,AbilitySpec.ActivationInfo.GetActivationPredictionKey());
				//未激活 or 将激活的 ability → 使用 GetPredictionKeyForNewAction()//
				//FPredictionKey PredictionKey = GetPredictionKeyForNewAction();
				//已激活的 ability 且是 InstancedPerActor → 首选从实例获取 PredictionKey
				FPredictionKey PredictionKey;
				// 获取实例化能力的主实例
				if (UGameplayAbility* AbilityInstance = AbilitySpec.GetPrimaryInstance())
				{
					PredictionKey = AbilityInstance->GetCurrentActivationInfo().GetActivationPredictionKey();
				}
				else
				{
					// fallback，未激活或未实例化或每次执行实例化
					UE_LOG(LogTemp, Warning, TEXT("AbilityInstance is null for tag %s, using fallback prediction key"), *InputTag.ToString());
					PredictionKey = GetPredictionKeyForNewAction(); 
				}
				InvokeReplicatedEvent(
					EAbilityGenericReplicatedEvent::InputPressed,
					AbilitySpec.Handle,
					PredictionKey
				);
			}
		}
	}
}

void UAbyssAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAbyssAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			/* 通知当前ability释放了，可以实现
			* virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) {};
			* 来处理释放后做什么处理
			*/
			AbilitySpecInputReleased(AbilitySpec);
			//下面已经在新版本不适用
			//InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle,AbilitySpec.ActivationInfo.GetActivationPredictionKey());
			
			//已激活的 ability 且是 InstancedPerActor → 首选从实例获取 PredictionKey
			FPredictionKey PredictionKey;
			// 获取实例化能力的主实例
			if (UGameplayAbility* AbilityInstance = AbilitySpec.GetPrimaryInstance())
			{
				PredictionKey = AbilityInstance->GetCurrentActivationInfo().GetActivationPredictionKey();
			}
			else
			{
				// fallback，未激活或未实例化或每次执行实例化
				UE_LOG(LogTemp, Warning, TEXT("AbilityInstance is null for tag %s, using fallback prediction key"), *InputTag.ToString());
				PredictionKey = GetPredictionKeyForNewAction(); 
			}
			
			// 正确触发 GAS 的 InputReleased 事件 
			InvokeReplicatedEvent(
				EAbilityGenericReplicatedEvent::InputReleased,
				AbilitySpec.Handle,
				PredictionKey
			);
			
			//TODO::处理按键释放
		}
	}
}

FGameplayTag UAbyssAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAbyssAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

void UAbyssAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	/*if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast(this);
	}*/
}

void UAbyssAbilitySystemComponent::SetAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level)
{
	if (IsValid(GetAvatarActor()) && !GetAvatarActor()->HasAuthority()) return;

	if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityClass))
	{
		AbilitySpec->Level = Level;
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UAbyssAbilitySystemComponent::AddToAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level)
{
	if (IsValid(GetAvatarActor()) && !GetAvatarActor()->HasAuthority()) return;

	if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityClass))
	{
		AbilitySpec->Level += Level;
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}


void UAbyssAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                      const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
}
