// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"

#include "InventorySlot.h"
#include "Components/ActorComponent.h"
#include "ESInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotChangedSignature);

USTRUCT()
struct FActorIgnoreSpec
{
	GENERATED_BODY()

	UPROPERTY()
	float TimeStamp;

	UPROPERTY()
	AActor* IgnoreActor;
};

/** Component that holds all equipable items, weapons and more */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHOOTERITEMS_API UESInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Sets default values for this component's properties */
	UESInventoryComponent(const FObjectInitializer& ObjectInitializer);

	/** Activates given slot */
	UFUNCTION(BlueprintCallable)
	void ActivateSlot(const int32& SlotIndex);

	/** Drops current slot item */
	UFUNCTION(BlueprintCallable)
	void DropActiveItem();

	/** Finds next slot with item */
	UFUNCTION(BlueprintCallable)
	bool FindNextValidSlotIndex(int32& OutIndex) const;
	
	/** Finds prev slot with item */
	UFUNCTION(BlueprintCallable)
	bool FindPreviousValidSlotIndex(int32& OutIndex) const;
	
	/** Active slot getter */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UInventorySlot* GetActiveSlot() const { return ActiveSlot; }

	/** Getter for currently active item */
	UFUNCTION(BlueprintCallable)
	class AESEquipableItemBase* GetActiveItem() const;

	UPROPERTY(BlueprintAssignable)
	FSlotChangedSignature OnSlotChanged;

protected:
	/** Called when the game starts */
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void UpdateIgnoreActors();

	UFUNCTION()
	bool IsIgnored(AActor* Actor);
	
	/** Ignore overlaps with actors */
	UPROPERTY()
	TArray<FActorIgnoreSpec> ItemsIgnoreArray;

	/** IgnoreDuration after item drop */
	UPROPERTY()
	float IgnoreDuration;
	
	/** Attaches item to owning character if not nullptr and sets in slot */
	void AssignNewItemToSlot(class UInventorySlot* InSlot, class AESEquipableItemBase* NewItem) const;

	/** Called when owning character begin overlap */
	UFUNCTION()
	void OnOwningCharacterBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                   const FHitResult& SweepResult);

	/** Called when owning character end overlap */
	UFUNCTION()
	void OnOwningCharacterEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** List of equipable items that owning character is overlapping with currently. Only contains not currently equipped items waiting for free slot. */
	UPROPERTY()
	TArray<class AESEquipableItemBase*> WaitingOverlappingItems;

	/** Preset layout of slots */
	UPROPERTY(EditDefaultsOnly, Category="ExampleShooter | Slot")
	TSoftObjectPtr<class UInventoryLayout> Layout;

	/** Init slots from data asset */
	void InitializeSlots();

	/** Returns false if slot not found. Finds first free slot of given type and Writes result in OutVar */
	bool FindEmptySlotOfType(const EInventorySlot& Type, UInventorySlot*& OutSlot) const;

	/** Currently active slot */
	UPROPERTY()
	class UInventorySlot* ActiveSlot;

	/** All inventory slots that inventory holds items in */
	UPROPERTY()
	TArray<class UInventorySlot*> InventorySlots;
};
