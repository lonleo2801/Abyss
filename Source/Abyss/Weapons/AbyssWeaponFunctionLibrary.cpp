// Copyright (c) 2025 Leon Lee


#include "AbyssWeaponFunctionLibrary.h"

#include "Data/AbyssDataRegistryManager.h"
#include "Data/RegistryTypeDefine.h"




bool UAbyssWeaponFunctionLibrary::GetWeaponDataSync(FName RowName, FAbyssWeaponDataRow& OutData)
{
	const FAbyssWeaponDataRow* FoundData = UAbyssDataRegistryManager::FindCachedSync<FAbyssWeaponDataRow>(RowName, AbyssRegistryType::Weapons);
	
	if (FoundData)
	{
		OutData = *FoundData;
		return true;
	}

	return false;
}

void UAbyssWeaponFunctionLibrary::GetWeaponDataAsync(FName RowName)
{
}


