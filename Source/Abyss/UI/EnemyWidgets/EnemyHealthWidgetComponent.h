// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "EnemyHealthWidgetComponent.generated.h"


class UEnemyHealthWidget;
class AAbyssEnemyBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ABYSS_API UEnemyHealthWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UEnemyHealthWidgetComponent();
	virtual void BeginPlay() override;

	void InitialHealthWidget(AAbyssEnemyBase* Enemy);

private:
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category= "Abyss|UI", meta = (AllowPrivateAccess = true))
	TSubclassOf<UEnemyHealthWidget> HealthWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UEnemyHealthWidget> HealthWidget;
};
