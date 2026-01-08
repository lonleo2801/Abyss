// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Data/DataTables/AbyssWeaponDataRow.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbyssWeaponFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssWeaponFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/** 获取武器数据（同步方式） */
	UFUNCTION(BlueprintCallable, Category="Abyss|Weapon|Registry")
	static bool GetWeaponDataSync(FName RowName, FAbyssWeaponDataRow& OutData);

	/** 获取武器数据（异步方式） */
	UFUNCTION(BlueprintCallable, Category="Abyss|Weapon|Registry")
	static void GetWeaponDataAsync(FName RowName);
	
};


