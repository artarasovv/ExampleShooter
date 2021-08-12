// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "ESItemBase.h"
#include "ESPickupBase.generated.h"

/** Base class for all pickups like heath pack, ammo box and other */
UCLASS(Abstract)
class SHOOTERITEMS_API AESPickupBase : public AESItemBase
{
	GENERATED_BODY()

public:
	AESPickupBase(const FObjectInitializer& ObjectInitializer);
	
protected:
	/** Respawn pickup time */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ExampleShooter | Respawn")
	float RespawnTime;
	
	virtual void OnPickedUp(TScriptInterface<IAbilitySystemInterface> AbilitySystem) override;
	virtual void OnDropped() override;

	/** Respawns pickup after respawn timer */
	UFUNCTION()
	virtual void Respawn();

private:
	/** Visual particle effect */
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* NiagaraComponent;

	/** We use this handle to store current respawn timer handle */
	UPROPERTY()
	FTimerHandle RespawnTimerHandle;

	/** We bind respawn function on this delegate */
	FTimerDelegate RespawnTimerDelegate;
};
