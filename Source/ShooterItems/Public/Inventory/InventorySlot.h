// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventorySlot.generated.h"

/** Item slot type */
UENUM(Blueprintable)
enum class EInventorySlot : uint8
{
	IS_Primary = 0 UMETA(Display="Primary"),
	IS_Secondary = 1 UMETA(Display="Secondary")
};

/** Inventory slot that can hold an item */
UCLASS()
class SHOOTERITEMS_API UInventorySlot : public UObject
{
	GENERATED_BODY()

public:
	UInventorySlot(const FObjectInitializer& ObjectInitializer);

	/** Activates slot */
	UFUNCTION(BlueprintCallable)
	virtual void ActivateSlot();

	/** Deactivates slot */
	UFUNCTION(BlueprintCallable)
	virtual void DeactivateSlot();

	/** Item getter */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class AESEquipableItemBase* GetItem() const { return Item; }

	/** Type getter */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EInventorySlot GetSlotType() const { return Type; }

	/** Checks slot is empty */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsEmpty() const { return !GetItem(); }
	
	/** Type setter */
	UFUNCTION(BlueprintCallable)
	void SetSlotType(const EInventorySlot& NewType);

	/** Clears item from slot */
	UFUNCTION(BlueprintCallable)
	void ClearSlot();

	/** Sets new item in slot */
	UFUNCTION(BlueprintCallable)
	void SetSlotItem(class AESEquipableItemBase* InItem);
	
private:
	/** Type of inventory slot */
	UPROPERTY(EditDefaultsOnly)
	EInventorySlot Type;

	/** Item actor in slot */
	UPROPERTY()
	class AESEquipableItemBase* Item;
};
