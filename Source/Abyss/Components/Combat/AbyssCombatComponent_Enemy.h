// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbyssCombatComponentBase.h"
#include "AbyssCombatComponent_Enemy.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ABYSS_API UAbyssCombatComponent_Enemy : public UAbyssCombatComponentBase
{
	GENERATED_BODY()

	virtual UAbilitySystemComponent* GetOwningAbilitySystemComponent() const override;
	virtual UAttributeSet* GetOwningAttributeSet() const override;
};
