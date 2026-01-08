// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthWidget.generated.h"

class UAbyssViewModel_EnemyAttributes;
class AAbyssEnemyBase;
/**
 * 
 */
UCLASS()
class ABYSS_API UEnemyHealthWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void SetOwnerEnemy(AAbyssEnemyBase* Enemy);
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AAbyssEnemyBase> OwnerEnemy;
	
};
