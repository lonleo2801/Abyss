// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "DataRegistrySubsystem.h"
#include "UObject/Object.h"
#include "AbyssDataRegistryManager.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssDataRegistryManager : public UObject
{
	GENERATED_BODY()

public:
	//寻找数据表中所有行，返回TArray用于遍历
	template<class T>
	static TArray<const T*> FindCachedAll(const FName RegistryType);

	//使用谓词遍历数据表中的所有行
	template<class T>
	static void ForEachCached(const FName RegistryType,TFunctionRef<void(const FName& Name, const T& Item)> Predicate);
	
	//同步寻找单个数据表行
	template<class T>
	static const T* FindCachedSync(const FName RowName, const FName RegistryType);
	
	//异步寻找单个数据表行
	static bool FindCachedAsync(const FName RowName, const FName RegistryType, const FDataRegistryItemAcquiredCallback& Delegate);
	
	//通过Tag同步寻找单个数据表行
	template<class T>
	static const T* FindCachedSyncByTag(const FGameplayTag& Tag, const FName RegistryType);

	//通过Tag异步寻找单个数据表行
	static bool FindCachedAsyncByTag(const FGameplayTag& Tag, const FName RegistryType, const FDataRegistryItemAcquiredCallback& Delegate);
	
};

template <class T>
TArray<const T*> UAbyssDataRegistryManager::FindCachedAll(const FName RegistryType)
{
	if (const UDataRegistrySubsystem* Subsystem = GEngine->GetEngineSubsystem<UDataRegistrySubsystem>())
	{
		UDataRegistry* Registry = Subsystem->GetRegistryForType(RegistryType);
		
		TCHAR *ErrorChar = RegistryType.ToString().GetCharArray().GetData();
		TArray<const T*> InputConfigTableRows;
		// * 把Registry中的每一行数据放到TArray里面 * //
		Registry->GetAllItems(ErrorChar,InputConfigTableRows);
		return InputConfigTableRows;
	}
	return TArray<const T*>();
}

template <class T>
void UAbyssDataRegistryManager::ForEachCached(const FName RegistryType,
	TFunctionRef<void(const FName& Name, const T& Item)> Predicate)
{
	if (const UDataRegistrySubsystem* Subsystem = GEngine->GetEngineSubsystem<UDataRegistrySubsystem>())
	{
		UDataRegistry* Registry = Subsystem->GetRegistryForType(RegistryType);
		
		Registry->ForEachCachedItem(RegistryType.ToString(),Predicate);
	}
}


template <class T>
const T* UAbyssDataRegistryManager::FindCachedSync(const FName RowName, const FName RegistryType)
{
	if (const UDataRegistrySubsystem* Subsystem = GEngine->GetEngineSubsystem<UDataRegistrySubsystem>())
	{
		FDataRegistryId RegistryId;
		RegistryId.ItemName = RowName;
		RegistryId.RegistryType = RegistryType;

		return Subsystem->GetCachedItem<T>(RegistryId);
	}
	return nullptr;
}


inline bool UAbyssDataRegistryManager::FindCachedAsync(const FName RowName, const FName RegistryType,
                                                       const FDataRegistryItemAcquiredCallback& Delegate)
{
	bool bSuccess=false;
	if (const UDataRegistrySubsystem* Subsystem = GEngine->GetEngineSubsystem<UDataRegistrySubsystem>())
	{
		FDataRegistryId RegistryId;
		RegistryId.ItemName = RowName;
		RegistryId.RegistryType = RegistryType;

		bSuccess = Subsystem->AcquireItem(RegistryId,Delegate);
	}
	return bSuccess;
}



template <class T>
const T* UAbyssDataRegistryManager::FindCachedSyncByTag(const FGameplayTag& Tag, const FName RegistryType)
{
	if (const UDataRegistrySubsystem* Subsystem = GEngine->GetEngineSubsystem<UDataRegistrySubsystem>())
	{
		FDataRegistryId RegistryId;
		RegistryId.ItemName = Tag.GetTagName();
		RegistryId.RegistryType = RegistryType;

		return Subsystem->GetCachedItem<T>(RegistryId);
	}
	return nullptr;
}

inline bool UAbyssDataRegistryManager::FindCachedAsyncByTag(const FGameplayTag& Tag, const FName RegistryType, const FDataRegistryItemAcquiredCallback& Delegate)
{
	bool bSuccess=false;
	if (const UDataRegistrySubsystem* Subsystem = GEngine->GetEngineSubsystem<UDataRegistrySubsystem>())
	{
		FDataRegistryId RegistryId;
		RegistryId.ItemName = Tag.GetTagName();
		RegistryId.RegistryType = RegistryType;

		bSuccess = Subsystem->AcquireItem(RegistryId,Delegate);
	}
	return bSuccess;
}

