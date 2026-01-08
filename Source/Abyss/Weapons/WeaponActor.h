// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "DataRegistryId.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

class AAbyssHeroBase;


UCLASS()
class ABYSS_API AWeaponActor : public AActor
{
	GENERATED_BODY()

public:
	AWeaponActor();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;
	
	// === Components ===
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* CombatCollision;

	// === Replicated Variables ===
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abyss|Weapon")
	FDataRegistryId WeaponId;

	UPROPERTY(Replicated)
	bool bPickedUp = false;

	UPROPERTY(ReplicatedUsing = OnRep_AttachedCharacter)
	TObjectPtr<AAbyssHeroBase> AttachedCharacter;
	
	UFUNCTION()
	void OnOverlapPickup(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp,
						 int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Server, Reliable)
	void ServerHandlePickup(AAbyssHeroBase* PickingHero);

	UFUNCTION()
	void OnRep_AttachedCharacter();
	
	void AttachToCharacter(AAbyssHeroBase* PickingHero);
	void DetachFromCharacter();
};
