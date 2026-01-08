// Copyright (c) 2025 Leon Lee


#include "AbyssAbilitySystemGlobals.h"

#include "Types/AbyssGameplayEffectContext.h"


FGameplayEffectContext* UAbyssAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FAbyssGameplayEffectContext();
}
