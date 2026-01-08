// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "Characters/AbyssCharacterBase.h"
#include "Components/ActorComponent.h"
#include "AbyssExtensionComponentBase.generated.h"


class AAbyssCharacterBase;
class UAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ABYSS_API UAbyssExtensionComponentBase : public UActorComponent
{
	GENERATED_BODY()

protected:
	template <class T>
	T* GetOwningPawn() const
	{
		static_assert(TPointerIsConvertibleFromTo<T,APawn>::Value,"'T' Template Parameter to GetPawn must be derived from APawn");
		return CastChecked<T>(GetOwner());
	}

	APawn* GetOwningPawn() const
	{
		return GetOwningPawn<APawn>();
	}

	template <class T>
	T* GetOwningController() const
	{
		static_assert(TPointerIsConvertibleFromTo<T,AController>::Value,"'T' Template Parameter to GetController must be derived from AController");
		return GetOwningPawn<APawn>()->GetController<T>();
	}

	template <class T>
	T* GetOwningPlayerState() const
	{
		static_assert(TPointerIsConvertibleFromTo<T,APlayerState>::Value,"'T' Template Parameter to GetPlayerState must be derived from APlayerState");
		return GetOwningPawn<APawn>()->GetPlayerState<T>();
	}
	
	virtual UAbilitySystemComponent* GetOwningAbilitySystemComponent() const
	{
		return nullptr;
	}
	
	virtual UAttributeSet* GetOwningAttributeSet() const
	{
		return nullptr;
	}
};
