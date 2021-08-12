// Copyright, Artem Tarasov, 2021


#include "Items/ESWeaponBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Net/UnrealNetwork.h"
#include "Targets/ESGATA_LineTrace.h"

AESWeaponBase::AESWeaponBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	WeaponIsFiringTag = FGameplayTag::RequestGameplayTag(TEXT("Weapon.Firing"));
}

void AESWeaponBase::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	const bool bASCValid = OwningAbilitySystem && IsValid(OwningAbilitySystem->GetAbilitySystemComponent());
	bool bMatchingFiringTag = false;
	if (bASCValid)
		bMatchingFiringTag = OwningAbilitySystem->GetAbilitySystemComponent()->HasMatchingGameplayTag(
			WeaponIsFiringTag);

	const bool bReplicateClipAmmo = bASCValid && !bMatchingFiringTag;

	DOREPLIFETIME_ACTIVE_OVERRIDE(AESWeaponBase, ClipAmmo, bReplicateClipAmmo);
}

void AESWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AESWeaponBase, ClipAmmo);
	DOREPLIFETIME(AESWeaponBase, MaxClipAmmo);
}

int32 AESWeaponBase::GetClipAmmo() const
{
	return ClipAmmo;
}

int32 AESWeaponBase::GetMaxClipAmmo() const
{
	return MaxClipAmmo;
}

void AESWeaponBase::SetClipAmmo(int32 NewClipAmmo)
{
	const int32 OldClipAmmo = ClipAmmo;
	ClipAmmo = NewClipAmmo;
	OnClipAmmoChanged.Broadcast(OldClipAmmo, ClipAmmo);
}

void AESWeaponBase::SetMaxClipAmmo(int32 NewMaxClipAmmo)
{
	const int32 OldMaxClipAmmo = MaxClipAmmo;
	MaxClipAmmo = NewMaxClipAmmo;
	OnMaxClipAmmoChanged.Broadcast(OldMaxClipAmmo, MaxClipAmmo);
}

AESGATA_LineTrace* AESWeaponBase::GetLineTraceTargetActor()
{
	if (LineTraceTargetActor)
	{
		return LineTraceTargetActor;
	}

	LineTraceTargetActor = GetWorld()->SpawnActor<AESGATA_LineTrace>();
	LineTraceTargetActor->SetOwner(this);
	return LineTraceTargetActor;
}

void AESWeaponBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (LineTraceTargetActor)
	{
		LineTraceTargetActor->Destroy();
	}

	Super::EndPlay(EndPlayReason);
}

void AESWeaponBase::OnRep_ClipAmmo(int32 OldClipAmmo)
{
	OnClipAmmoChanged.Broadcast(OldClipAmmo, ClipAmmo);
}

void AESWeaponBase::OnRep_MaxClipAmmo(int32 OldMaxClipAmmo)
{
	OnMaxClipAmmoChanged.Broadcast(OldMaxClipAmmo, MaxClipAmmo);
}
