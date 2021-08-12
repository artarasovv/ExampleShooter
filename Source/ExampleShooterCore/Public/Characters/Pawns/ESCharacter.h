// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/WeaponInterface.h"
#include "Interfaces/InventoryInterface.h"
#include "Interfaces/ItemAttachInterface.h"
#include "ESCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponChangedSignature, class AESWeaponBase*, NewWeapon);

/** Base character class */
UCLASS()
class EXAMPLESHOOTERCORE_API AESCharacter : public ACharacter, public IAbilitySystemInterface,
                                            public IInventoryInterface,
                                            public IWeaponInterface,
											public IItemAttachInterface
{
	GENERATED_BODY()

public:
	AESCharacter(const FObjectInitializer& ObjectInitializer);

	/** Begin IAbilitySystemInterface implementation */
	UFUNCTION(BlueprintCallable)
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Begin IInventoryInterface implementation */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	class UESInventoryComponent* GetInventoryComponent() const;
	virtual class UESInventoryComponent* GetInventoryComponent_Implementation() const override;

	/** Begin IWeaponInterface implementation */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetWeaponMuzzleTransform(FTransform& OutTransform) const;
	virtual bool GetWeaponMuzzleTransform_Implementation(FTransform& OutTransform) const override;

	/** Begin IItemAttachInterface implementation */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	class USkeletalMeshComponent* GetNetRelevantMesh() const;
	virtual class USkeletalMeshComponent* GetNetRelevantMesh_Implementation() const override;

	/** Getter for FPS arms mesh */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class USkeletalMeshComponent* GetArmsMesh() const { return ArmsMesh; }

	/** Getter for FPS camera */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }

	/** Used to server setup ASC when pawn is possessed by controller */
	virtual void PossessedBy(AController* NewController) override;

	/** Getter for current weapon */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class AESWeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }

	/** Binds abilities input */
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientBindASCInput() const;
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/** Calls when game is started */
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentWeapon)
	class AESWeaponBase* CurrentWeapon;

	UPROPERTY(BlueprintAssignable)
	FWeaponChangedSignature OnWeaponChanged;

	/** Character abilities granted on begin play */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ExampleShooter | Abilities")
	TArray<TSubclassOf<class UESGameplayAbility>> StartupAbilities;

	/** Default attributes gameplay effect. Applies on begin play */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ExampleShooter | Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	/** Startup gameplay effects. Applies on begin play */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ExampleShooter | Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

	UFUNCTION()
	virtual void OnRep_CurrentWeapon(class AESWeaponBase* OldCurrentWeapon);

private:
	UFUNCTION()
	void OnInventorySlotChanged();
	
	/** FPS arms mesh */
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess = "true"), Category= "ExampleShooter | Character")
	class USkeletalMeshComponent* ArmsMesh;

	/** FPS camera component */
	UPROPERTY(VisibleAnywhere, Category= "ExampleShooter | Character")
	class UCameraComponent* CameraComponent;

	/** Inventory component cached */
	UPROPERTY(VisibleAnywhere, Category="ExampleShooter | Character")
	class UESInventoryComponent* InventoryComponent;

	/** ASC cached */
	UPROPERTY()
	class UESAbilitySystemComponent* AbilitySystemComponent;

	/** AttributeSet cached */
	UPROPERTY()
	class UESAttributeSet* AttributeSet;

	/** Ammo AttributeSet cached */
	UPROPERTY()
	class UESAmmoAttributeSet* AmmoAttributeSet;
};
