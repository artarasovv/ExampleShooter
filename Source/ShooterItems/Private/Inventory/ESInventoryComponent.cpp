// Copyright, Artem Tarasov, 2021


#include "Inventory/ESInventoryComponent.h"
#include "AbilitySystemInterface.h"
#include "ESEquipableItemBase.h"
#include "ESPickupBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Inventory/InventoryLayout.h"
#include "Inventory/InventorySlot.h"
#include "Kismet/KismetMathLibrary.h"

UESInventoryComponent::UESInventoryComponent(const FObjectInitializer& ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	IgnoreDuration = 1.5f;
}

void UESInventoryComponent::ActivateSlot(const int32& SlotIndex)
{
	if (!InventorySlots.IsValidIndex(SlotIndex))
		return;

	UInventorySlot* CurrentSlot = ActiveSlot;
	if (CurrentSlot)
	{
		CurrentSlot->DeactivateSlot();
	}

	UInventorySlot* NewSlot = InventorySlots[SlotIndex];
	if (NewSlot)
	{
		NewSlot->ActivateSlot();
		ActiveSlot = NewSlot;
		OnSlotChanged.Broadcast();
	}
}

void UESInventoryComponent::UpdateIgnoreActors()
{
	TArray<int32> RemoveIndexes;
	
	for (int32 Index = 0; Index < ItemsIgnoreArray.Num(); Index++)
	{
		const FActorIgnoreSpec& CurSpec = ItemsIgnoreArray[Index];
		if (!CurSpec.IgnoreActor || GetWorld()->GetTimeSeconds() - CurSpec.TimeStamp >= IgnoreDuration)
		{
			RemoveIndexes.Add(Index);
		}
	}

	for (const int32 RemoveIndex : RemoveIndexes)
		ItemsIgnoreArray.RemoveAt(RemoveIndex);
}

bool UESInventoryComponent::IsIgnored(AActor* Actor)
{
	for (const FActorIgnoreSpec& Spec : ItemsIgnoreArray)
	{
		if (Spec.IgnoreActor == Actor)
		{
			return true;
		}
	}

	return false;
}

void UESInventoryComponent::DropActiveItem()
{
	if (!GetActiveSlot() || !GetActiveItem())
		return;

	FActorIgnoreSpec IgnoreSpec;
	IgnoreSpec.TimeStamp = GetWorld()->GetTimeSeconds();
	IgnoreSpec.IgnoreActor = GetActiveItem();
	
	ItemsIgnoreArray.Add(IgnoreSpec);

	GetActiveItem()->UnEquipItem();
	GetActiveSlot()->ClearSlot();

	for (AESEquipableItemBase* Item : WaitingOverlappingItems)
		// As slot is clear now we can find waiting item to set in active slot 
	{
		if (Item->GetSlotType() == GetActiveSlot()->GetSlotType())
		{
			AssignNewItemToSlot(GetActiveSlot(), Item);
			break;
		}
	}

	if (GetActiveItem()) // Remove from waiting list 
		WaitingOverlappingItems.Remove(GetActiveItem());
	else
	{
		int32 NewIndex;
		if (FindNextValidSlotIndex(NewIndex))
		{
			ActivateSlot(NewIndex);
		}
	}

	OnSlotChanged.Broadcast();
}

bool UESInventoryComponent::FindNextValidSlotIndex(int32& OutIndex) const
{
	int32 CurrentSlotIndex = -1;
	if (GetActiveSlot())
	{
		InventorySlots.Find(GetActiveSlot(), CurrentSlotIndex);
	}

	for (int32 Index = CurrentSlotIndex + 1; Index < InventorySlots.Num(); ++Index)
	{
		if (InventorySlots.IsValidIndex(Index) && InventorySlots[Index]->GetItem())
		{
			OutIndex = Index;
			return true;
		}
	}

	for (int32 Index = 0; Index < CurrentSlotIndex; ++Index)
	{
		if (InventorySlots.IsValidIndex(Index) && InventorySlots[Index]->GetItem())
		{
			OutIndex = Index;
			return true;
		}
	}

	return false;
}

bool UESInventoryComponent::FindPreviousValidSlotIndex(int32& OutIndex) const
{
	int32 CurrentSlotIndex = InventorySlots.Num();
	if (GetActiveSlot())
	{
		InventorySlots.Find(GetActiveSlot(), CurrentSlotIndex);
	}

	for (int32 Index = CurrentSlotIndex - 1; Index >= 0; --Index)
	{
		if (InventorySlots.IsValidIndex(Index) && InventorySlots[Index]->GetItem())
		{
			OutIndex = Index;
			return true;
		}
	}

	for (int32 Index = InventorySlots.Num() - 1; Index > CurrentSlotIndex; --Index)
	{
		if (InventorySlots.IsValidIndex(Index) && InventorySlots[Index]->GetItem())
		{
			OutIndex = Index;
			return true;
		}
	}

	return false;
}

AESEquipableItemBase* UESInventoryComponent::GetActiveItem() const
{
	if (!GetActiveSlot())
		return nullptr;

	return GetActiveSlot()->GetItem();
}

void UESInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeSlots();

	//Bind to owning character collision
	if (ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner()))
	{
		OwningCharacter->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(
			this, &UESInventoryComponent::OnOwningCharacterBeginOverlap);
		OwningCharacter->GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(
			this, &UESInventoryComponent::OnOwningCharacterEndOverlap);
	}
}

void UESInventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (UInventorySlot* Slot : InventorySlots)
	{
		if (Slot)
		{
			if (Slot->GetItem())
			{
				Slot->GetItem()->Destroy();
			}

			Slot->ConditionalBeginDestroy();
			Slot = nullptr;
		}
	}

	GEngine->ForceGarbageCollection(true);
}

void UESInventoryComponent::AssignNewItemToSlot(UInventorySlot* InSlot, AESEquipableItemBase* NewItem) const
{
	if (!InSlot || !NewItem)
		return;

	InSlot->SetSlotItem(NewItem);
}

void UESInventoryComponent::OnOwningCharacterBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                          bool bFromSweep, const FHitResult& SweepResult)
{
	UpdateIgnoreActors();
	if (IsIgnored(OtherActor))
		return;
	
	IAbilitySystemInterface* OwningASC = Cast<IAbilitySystemInterface>(GetOwner());
	TScriptInterface<IAbilitySystemInterface> ScriptInterface;
	ScriptInterface.SetInterface(OwningASC);
	ScriptInterface.SetObject(GetOuter());
	
	if (!OwningASC)
		return;

	AESItemBase* ItemBase = Cast<AESItemBase>(OtherActor);
	if (!ItemBase || !ItemBase->CanBePickedUp(ScriptInterface))
		return;
	
	if (GetOwnerRole() == ROLE_Authority) //SERVER ONLY. Instantly pick up AESPickupBase
	{
		if (AESPickupBase* Pickup = Cast<AESPickupBase>(OtherActor))
		{
			Pickup->PickUpItem(ScriptInterface);
			return;
		}
	}
	
	AESEquipableItemBase* EquipableItem = Cast<AESEquipableItemBase>(OtherActor);

	if (!EquipableItem) //Only react to equipable items
		return;

	if (WaitingOverlappingItems.Contains(EquipableItem)) //Don't react twice
		return;

	UInventorySlot* OutSlot = nullptr;
	if (FindEmptySlotOfType(EquipableItem->GetSlotType(), OutSlot))
	{
		AssignNewItemToSlot(OutSlot, EquipableItem);

		if (!GetActiveItem() || OutSlot == GetActiveSlot())
		{
			const int32 SlotIndex = InventorySlots.Find(OutSlot);
			ActivateSlot(SlotIndex);
		}
		else
		{
			EquipableItem->SetActorHiddenInGame(true);
		}
	}
	else if (EquipableItem != GetActiveItem())
	{
		WaitingOverlappingItems.Add(EquipableItem);
	}
}

void UESInventoryComponent::OnOwningCharacterEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AESEquipableItemBase* EquipableItem = Cast<AESEquipableItemBase>(OtherActor);

	if (!EquipableItem) //Only react to equipable items
		return;

	if (!WaitingOverlappingItems.Contains(EquipableItem))
		return;

	WaitingOverlappingItems.Remove(EquipableItem);
}

void UESInventoryComponent::InitializeSlots()
{
	UInventoryLayout* InitLayout = Layout.LoadSynchronous();
	if (InitLayout)
	{
		for (const EInventorySlot& Type : InitLayout->InventorySlotsTypes)
		{
			UInventorySlot* NewSlot = NewObject<UInventorySlot>(GetOwner());
			NewSlot->SetSlotType(Type);
			InventorySlots.Add(NewSlot);
		}
	}
}

bool UESInventoryComponent::FindEmptySlotOfType(const EInventorySlot& Type, UInventorySlot*& OutSlot) const
{
	for (UInventorySlot* Slot : InventorySlots)
	{
		if (Slot->GetSlotType() == Type && Slot->IsEmpty())
		{
			OutSlot = Slot;
			return true;
		}
	}

	return false;
}
