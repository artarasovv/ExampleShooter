// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "Abilities/ESGameplayAbility.h"
#include "ESGA_DropItem.generated.h"

/** Drop current equipped inventory item */
UCLASS()
class SHOOTERITEMS_API UESGA_DropItem : public UESGameplayAbility
{
	GENERATED_BODY()

public:
	UESGA_DropItem();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
};
