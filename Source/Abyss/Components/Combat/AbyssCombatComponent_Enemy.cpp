// Copyright (c) 2025 Leon Lee


#include "AbyssCombatComponent_Enemy.h"

#include "Characters/Enemy/AbyssEnemyBase.h"


UAbilitySystemComponent* UAbyssCombatComponent_Enemy::GetOwningAbilitySystemComponent() const
{
	return GetOwningPawn<AAbyssEnemyBase>()->GetAbilitySystemComponent();
}

UAttributeSet* UAbyssCombatComponent_Enemy::GetOwningAttributeSet() const
{
	return GetOwningPawn<AAbyssEnemyBase>()->GetAttributeSet();
}
