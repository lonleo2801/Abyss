// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "DataRegistrySubsystem.h"
#include "EnhancedInputComponent.h"
#include "Data/AbyssDataRegistryManager.h"
#include "Data/RegistryTypeDefine.h"
#include "Data/DataTables/AbyssInputConfigRow.h"
#include "AbyssInputComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ABYSS_API UAbyssInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserClass,typename PressedFuncType,typename ReleasedFuncType, typename HeldFuncType>
	void BindAbilityActions(UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UAbyssInputComponent::BindAbilityActions(UserClass* Object, PressedFuncType PressedFunc,
	ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	TArray<const FAbyssInputConfigRow*> InputConfigTableRows = UAbyssDataRegistryManager::FindCachedAll<FAbyssInputConfigRow>(AbyssRegistryType::InputConfig);
	for (const FAbyssInputConfigRow* Row : InputConfigTableRows)
	{
		if (PressedFunc)
		{
			BindAction(Row->InputAction, ETriggerEvent::Started,Object,PressedFunc,Row->InputTag);
		}
		if (ReleasedFunc)
		{
			BindAction(Row->InputAction, ETriggerEvent::Completed,Object,ReleasedFunc,Row->InputTag);
		}
		if (HeldFunc)
		{
			BindAction(Row->InputAction, ETriggerEvent::Triggered,Object,HeldFunc,Row->InputTag);
		}
	}
}
