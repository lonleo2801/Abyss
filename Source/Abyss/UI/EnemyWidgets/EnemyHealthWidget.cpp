// Copyright (c) 2025 Leon Lee


#include "EnemyHealthWidget.h"

#include "Characters/Enemy/AbyssEnemyBase.h"


void UEnemyHealthWidget::SetOwnerEnemy(AAbyssEnemyBase* Enemy)
{
	if (Enemy)
	{
		OwnerEnemy = Enemy;
	}
}
	
