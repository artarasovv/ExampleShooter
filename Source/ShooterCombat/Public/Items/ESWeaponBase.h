// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "ESEquipableItemBase.h"
#include "ESWeaponBase.generated.h"

/** Declare base delegate for ammo changed */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponAmmoChangedDelegate, int32, OldValue, int32, NewValue);

/** Declare ammo type enum */
UENUM(BlueprintType, Blueprintable)
enum class EAmmoType : uint8
{
	AT_Rifle UMETA(Display="Rifle"),
	AT_Sniper UMETA(Display="Sniper"),
	AT_Shotgun UMETA(Display="Shotgun")
};

/** Base class for all equipable weapons */
UCLASS(Abstract)
class SHOOTERCOMBAT_API AESWeaponBase : public AESEquipableItemBase
{
	GENERATED_BODY()

public:
	AESWeaponBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, Category = "ExampleShooter|Weapon")
	FWeaponAmmoChangedDelegate OnClipAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "ExampleShooter|Weapon")
	FWeaponAmmoChangedDelegate OnMaxClipAmmoChanged;

	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "ExampleShooter|Weapon")
	virtual int32 GetClipAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "ExampleShooter|Weapon")
	virtual int32 GetMaxClipAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "ExampleShooter|Weapon")
	virtual void SetClipAmmo(int32 NewClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "ExampleShooter|Weapon")
	virtual void SetMaxClipAmmo(int32 NewMaxClipAmmo);

	/** Getter for LineTraceTargetActor. Spawns it if it doesn't exist yet. */
	UFUNCTION(BlueprintCallable, Category = "ExampleShooter|Targeting")
	class AESGATA_LineTrace* GetLineTraceTargetActor();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	FGameplayTag WeaponIsFiringTag;

	UPROPERTY()
	class AESGATA_LineTrace* LineTraceTargetActor;
	
	/** How much ammo in the clip the gun starts with */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, ReplicatedUsing=OnRep_ClipAmmo, Category = "ExampleShooter|Weapon|Ammo")
	int32 ClipAmmo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ExampleShooter|Weapon|Ammo")
	EAmmoType AmmoType;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, ReplicatedUsing=OnRep_MaxClipAmmo, Category = "ExampleShooter|Weapon|Ammo")
	int32 MaxClipAmmo;

	UFUNCTION()
	virtual void OnRep_ClipAmmo(int32 OldClipAmmo);

	UFUNCTION()
	virtual void OnRep_MaxClipAmmo(int32 OldMaxClipAmmo);
};
