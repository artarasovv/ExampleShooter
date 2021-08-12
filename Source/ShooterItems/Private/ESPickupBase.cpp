// Copyright, Artem Tarasov, 2021


#include "ESPickupBase.h"
#include "NiagaraComponent.h"

AESPickupBase::AESPickupBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	NiagaraComponent->SetupAttachment(GetRootComponent());
	NiagaraComponent->SetIsReplicated(true);

	RespawnTime = 3.f;

	RespawnTimerDelegate.BindUObject(this, &AESPickupBase::Respawn);
}

void AESPickupBase::OnPickedUp(TScriptInterface<IAbilitySystemInterface> AbilitySystem)
{
	Super::OnPickedUp(AbilitySystem);

	TurnCollisionOff();
	NiagaraComponent->SetVisibility(false);

	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, RespawnTimerDelegate, RespawnTime, false);

	//Drop item to pickup again after respawn
	DropItem();
}

void AESPickupBase::OnDropped()
{
	// Do not remove any effects or gameplay abilities
}

void AESPickupBase::Respawn()
{
	NiagaraComponent->SetVisibility(true);
	TurnCollisionOn();
}
