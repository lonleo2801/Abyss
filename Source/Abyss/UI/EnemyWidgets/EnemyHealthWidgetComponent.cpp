// Copyright (c) 2025 Leon Lee


#include "EnemyHealthWidgetComponent.h"

#include "EnemyHealthWidget.h"


UEnemyHealthWidgetComponent::UEnemyHealthWidgetComponent()
{
}

void UEnemyHealthWidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UEnemyHealthWidgetComponent::InitialHealthWidget(AAbyssEnemyBase* Enemy)
{
	check(HealthWidgetClass);
	HealthWidget = CreateWidget<UEnemyHealthWidget>(GetWorld(),HealthWidgetClass);
	HealthWidget->SetOwnerEnemy(Enemy);
	SetWidget(HealthWidget);
}


