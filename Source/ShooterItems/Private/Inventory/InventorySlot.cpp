// Copyright, Artem Tarasov, 2021


#include "Inventory/InventorySlot.h"

#include "AbilitySystemInterface.h"
#include "ESEquipableItemBase.h"

UInventorySlot::UInventorySlot(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UInventorySlot::ActivateSlot()
{
	if (Item) //Turn on visibility on if item exists in world
	{
		Item->SetActorHiddenInGame(false);
		Item->EquipItem();
	}
}

void UInventorySlot::DeactivateSlot()
{
	if (Item) //Turn on visibility off if item exists in world
	{
		Item->SetActorHiddenInGame(true);
		Item->UnEquipItem();
	}
}

void UInventorySlot::SetSlotType(const EInventorySlot& NewType)
{
	if (Item) //Cant change slot type if holding item
		return;

	Type = NewType;
}

void UInventorySlot::ClearSlot()
{
	if (!Item)
		return;

	Item->DropItem();
	Item = nullptr;
}

void UInventorySlot::SetSlotItem(AESEquipableItemBase* InItem)
{
	if (Item || !InItem) //Cant set new item while holding current item
		return;

	if (IAbilitySystemInterface* OwningASC = Cast<IAbilitySystemInterface>(GetOuter()))
	{
		TScriptInterface<IAbilitySystemInterface> ScriptInterface;
		ScriptInterface.SetInterface(OwningASC);
		ScriptInterface.SetObject(GetOuter());

		if (InItem->CanBePickedUp(ScriptInterface))
		{
			Item = InItem;
			Item->PickUpItem(ScriptInterface);
		}
	}
}
