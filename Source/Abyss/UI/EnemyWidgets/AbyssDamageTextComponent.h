// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "AbyssDamageTextComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ABYSS_API UAbyssDamageTextComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void SetDamageText(float Damage,bool bBlockHit,bool bCritHit);

};
