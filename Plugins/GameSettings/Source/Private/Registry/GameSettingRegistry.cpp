// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameSettingRegistry.h"

#include "GameSettingCollection.h"
#include "GameSettingAction.h"
#include "UObject/WeakObjectPtr.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameSettingRegistry)

#define LOCTEXT_NAMESPACE "GameSetting"

//--------------------------------------
// UGameSettingRegistry
//--------------------------------------

UGameSettingRegistry::UGameSettingRegistry()
{
}

void UGameSettingRegistry::Initialize(ULocalPlayer* InLocalPlayer)
{
	OwningLocalPlayer = InLocalPlayer;
	OnInitialize(InLocalPlayer);

	//UGameFeaturesSubsystem
}

void UGameSettingRegistry::Regenerate()
{
	for (UGameSetting* Setting : RegisteredSettings)
	{
		// 标记回收
		Setting->MarkAsGarbage();
	}
	// 重置容器
	RegisteredSettings.Reset();
	TopLevelSettings.Reset();

	OnInitialize(OwningLocalPlayer);
}

bool UGameSettingRegistry::IsFinishedInitializing() const
{
	bool bReady = true;
	for (UGameSetting* Setting : RegisteredSettings)
	{
		if (!Setting->IsReady())
		{
			bReady = false;
			break;
		}
	}

	return bReady;
}

void UGameSettingRegistry::SaveChanges()
{

}

void UGameSettingRegistry::GetSettingsForFilter(const FGameSettingFilterState& FilterState, TArray<UGameSetting*>& InOutSettings)
{
	// 准备好要过滤游戏设置
	TArray<UGameSetting*> RootSettings;

	// 如果游戏设置过滤状态中有根设置的话 就使用它的
	if (FilterState.GetSettingRootList().Num() > 0)
	{
		RootSettings.Append(FilterState.GetSettingRootList());
	}
	else
	{
		// 过滤器中没有的话,就使用我们自身的顶级设置
		RootSettings.Append(TopLevelSettings);
	}

	// 遍历根设置
	for (UGameSetting* TopLevelSetting : RootSettings)
	{
		// 是否可以转换城游戏设置的集合
		if (const UGameSettingCollection* TopLevelCollection = Cast<UGameSettingCollection>(TopLevelSetting))
		{
			// 如果是集合的话 则调用集合的处理方式
			TopLevelCollection->GetSettingsForFilter(FilterState, InOutSettings);
		}
		else
		{
			// 如果不是集合 则直接判定是否可以通过过滤
			if (FilterState.DoesSettingPassFilter(*TopLevelSetting))
			{
				InOutSettings.Add(TopLevelSetting);
			}
		}
	}
}

UGameSetting* UGameSettingRegistry::FindSettingByDevName(const FName& SettingDevName)
{
	for (UGameSetting* Setting : RegisteredSettings)
	{
		if (Setting->GetDevName() == SettingDevName)
		{
			return Setting;
		}
	}

	return nullptr;
}

void UGameSettingRegistry::RegisterSetting(UGameSetting* InSetting)
{
	if (InSetting)
	{
		// 添加到顶层中
		TopLevelSettings.Add(InSetting);
		// 指定游戏设置的注册器
		InSetting->SetRegistry(this);
		// 内部注册绑定代理
		RegisterInnerSettings(InSetting);
	}
}

void UGameSettingRegistry::RegisterInnerSettings(UGameSetting* InSetting)
{
	// 把游戏设置的变更绑定到自己身上
	InSetting->OnSettingChangedEvent.AddUObject(this, &ThisClass::HandleSettingChanged);
	InSetting->OnSettingAppliedEvent.AddUObject(this, &ThisClass::HandleSettingApplied);
	InSetting->OnSettingEditConditionChangedEvent.AddUObject(this, &ThisClass::HandleSettingEditConditionsChanged);

	// Not a fan of this, but it makes sense to aggregate action events for simplicity.
	// 不太喜欢这种做法，但为了便于操作，将动作事件进行汇总是合理的。
	if (UGameSettingAction* ActionSetting = Cast<UGameSettingAction>(InSetting))
	{
		ActionSetting->OnExecuteNamedActionEvent.AddUObject(this, &ThisClass::HandleSettingNamedAction);
	}
	// Not a fan of this, but it makes sense to aggregate navigation events for simplicity.
	// 不太喜欢这种做法，但为了便于操作，将导航事件进行汇总是合理的。
	else if (UGameSettingCollectionPage* NewPageCollection = Cast<UGameSettingCollectionPage>(InSetting))
	{
		// 这里是从UMG点击过来的 void UGameSettingCollectionPage::ExecuteNavigation()
		// UMG 是通过反射生成的
		NewPageCollection->OnExecuteNavigationEvent.AddUObject(this, &ThisClass::HandleSettingNavigation);
	}

#if !UE_BUILD_SHIPPING
	// 避免重复注册
	ensureAlwaysMsgf(!RegisteredSettings.Contains(InSetting), TEXT("This setting has already been registered!"));
	// 名字必须唯一
	ensureAlwaysMsgf(nullptr == RegisteredSettings.FindByPredicate([&InSetting](UGameSetting* ExistingSetting) { return InSetting->GetDevName() == ExistingSetting->GetDevName(); }), TEXT("A setting with this DevName has already been registered!  DevNames must be unique within a registry."));
#endif

	RegisteredSettings.Add(InSetting);

	for (UGameSetting* ChildSetting : InSetting->GetChildSettings())
	{
		// 递归注册子设置 不再添加到TopLevelSettings中了
		RegisterInnerSettings(ChildSetting);
	}
}

void UGameSettingRegistry::HandleSettingApplied(UGameSetting* Setting)
{
	OnSettingApplied(Setting);
}

void UGameSettingRegistry::HandleSettingChanged(UGameSetting* Setting, EGameSettingChangeReason Reason)
{
	OnSettingChangedEvent.Broadcast(Setting, Reason);
}

void UGameSettingRegistry::HandleSettingEditConditionsChanged(UGameSetting* Setting)
{
	OnSettingEditConditionChangedEvent.Broadcast(Setting);
}

void UGameSettingRegistry::HandleSettingNamedAction(UGameSetting* Setting, FGameplayTag GameSettings_Action_Tag)
{
	OnSettingNamedActionEvent.Broadcast(Setting, GameSettings_Action_Tag);
}

void UGameSettingRegistry::HandleSettingNavigation(UGameSetting* Setting)
{
	OnExecuteNavigationEvent.Broadcast(Setting);
}

#undef LOCTEXT_NAMESPACE

