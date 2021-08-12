// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "ESItemBase.h"
#include "Inventory/InventorySlot.h"

#include "ESEquipableItemBase.generated.h"

/** Base class for all items that can be equipped */
UCLASS(Abstract)
class SHOOTERITEMS_API AESEquipableItemBase : public AESItemBase
{
	GENERATED_BODY()

public:
	AESEquipableItemBase(const FObjectInitializer& ObjectInitializer);

	/** Getter for skeletal mesh */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class USkeletalMeshComponent* GetSkeletalMesh() const { return SkeletalMesh; }

	/** Getter for ArmsAttachTransform */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FTransform GetArmsAttachTransform() const { return ArmsAttachTransform; }

	/** Getter for ArmsAttachTransform */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FTransform GetBodyAttachTransform() const { return BodyAttachTransform; }
	
	/** Getter for AttachBone */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FName GetAttachBone() const { return AttachBone; }

	/** Getter for SlotType */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EInventorySlot GetSlotType() const { return SlotType; }
	
protected:
	/** Calls when item is picked up. Does OnPickedUp Job */
	virtual void OnPickedUp(TScriptInterface<class IAbilitySystemInterface> AbilitySystem) override;

	/** Calls when item is dropped. Does OnDropped Job */
	virtual void OnDropped() override;
	
	/** Calls when item is equipped. Does OnEquipped Job */
	virtual void OnEquipped();
	
	/** Calls when item is UnEquipped. Does OnUnEquipped Job */
	virtual void OnUnEquipped();

	/** Gameplay effects will be applied to owner when item is equipped */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ExampleShooter | Item | Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> EquippedGameplayEffects;

	/** Gameplay abilities will be granted to owner when item is equipped */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ExampleShooter | Item | Abilities")
	TArray<TSubclassOf<class UESGameplayAbility>> EquippedGameplayAbilities;

	/** Relative attach transform */
	UPROPERTY(EditDefaultsOnly, Category="ExampleShooter | Item | Attach")
	FTransform ArmsAttachTransform;

	/** Relative attach transform */
	UPROPERTY(EditDefaultsOnly, Category="ExampleShooter | Item | Attach")
	FTransform BodyAttachTransform;
	
	/** Bone that actor will be attached to */
	UPROPERTY(EditDefaultsOnly, Category="ExampleShooter | Item | Attach")
	FName AttachBone;

	/** Turn on PickUp collision */
	virtual void TurnCollisionOn() const override;
	
	/** Turn off PickUp collision */
	virtual void TurnCollisionOff() const override;
	
public:
	/** Equips item */
	void EquipItem();

	/** UnEquips item */
	void UnEquipItem();
	
private:
	/** Attaches item to owner on all clients. Passing Replicated owner from server to not replicate extra properties to improve bandwidth */
	UFUNCTION(NetMulticast, Reliable)
	void MultiAttachOnPickUp(UObject* ASCOwner);

	/** Detaches item from owner on all clients. Passing Replicated owner from server to not replicate extra properties to improve bandwidth */
	UFUNCTION(NetMulticast, Reliable)
	void MultiDetachOnDrop(UObject* ASCOwner);
	
	/** Magnitude of impulse applied to item when it dropped */
	UPROPERTY(EditDefaultsOnly, Category="ExampleShooter | Item | Attach")
	float DropImpulseMagnitude;
	
	/** Type of slot that item can be equipped to */
	UPROPERTY(EditDefaultsOnly, Category="ExampleShooter | Slot")
	EInventorySlot SlotType;
	
	/** Visual representation of Equipable item */
	UPROPERTY(VisibleAnywhere, Category="ExampleShooter | Mesh")
	class USkeletalMeshComponent* SkeletalMesh;
	
	/** Equipped gameplay effects handles. Used to remove effects on item UnEquip */
	TArray<FActiveGameplayEffectHandle> EquipActiveEffects;

	/** Equipped gameplay abilities handles. Used to remove effects on item UnEquip */
	TArray<FGameplayAbilitySpecHandle> EquipActiveAbilities;
};
