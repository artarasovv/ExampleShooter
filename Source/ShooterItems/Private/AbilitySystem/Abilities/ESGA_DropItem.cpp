// Copyright, Artem Tarasov, 2021


#include "AbilitySystem/Abilities/ESGA_DropItem.h"
#include "Interfaces/InventoryInterface.h"
#include "Inventory/ESInventoryComponent.h"

UESGA_DropItem::UESGA_DropItem()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Ability.Item.Drop"));

	AbilityTriggers.Add(TriggerData);
}

void UESGA_DropItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

		IInventoryInterface::Execute_GetInventoryComponent(ActorInfo->AvatarActor.Get())->DropActiveItem();
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}
