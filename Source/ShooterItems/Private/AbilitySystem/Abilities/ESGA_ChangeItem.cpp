// Copyright, Artem Tarasov, 2021


#include "AbilitySystem/Abilities/ESGA_ChangeItem.h"

#include "Interfaces/InventoryInterface.h"
#include "Inventory/ESInventoryComponent.h"

UESGA_ChangeItem::UESGA_ChangeItem()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;

	FAbilityTriggerData NextTriggerData;
	NextTriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	NextTriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Ability.Item.Next"));

	AbilityTriggers.Add(NextTriggerData);

	FAbilityTriggerData PrevTriggerData;
	PrevTriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	PrevTriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Ability.Item.Previous"));

	AbilityTriggers.Add(PrevTriggerData);
}

void UESGA_ChangeItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}

		if (!ActorInfo->AvatarActor.Get()->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}

		bool SlotFindResult = false;
		int32 NewSlotIndex = -1;

		if (TriggerEventData->EventTag == FGameplayTag::RequestGameplayTag("Event.Ability.Item.Next"))
		{
			SlotFindResult = IInventoryInterface::Execute_GetInventoryComponent(ActorInfo->AvatarActor.Get())->
				FindNextValidSlotIndex(NewSlotIndex);
		}
		else if (TriggerEventData->EventTag == FGameplayTag::RequestGameplayTag("Event.Ability.Item.Previous"))
		{
			SlotFindResult = IInventoryInterface::Execute_GetInventoryComponent(ActorInfo->AvatarActor.Get())->
				FindPreviousValidSlotIndex(NewSlotIndex);
		}

		if (SlotFindResult)
		{
			IInventoryInterface::Execute_GetInventoryComponent(ActorInfo->AvatarActor.Get())->
				ActivateSlot(NewSlotIndex);
		}

		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}
