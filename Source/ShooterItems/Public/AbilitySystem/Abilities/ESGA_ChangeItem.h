// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "Abilities/ESGameplayAbility.h"
#include "ESGA_ChangeItem.generated.h"

/** Selects next or previous item slot */
UCLASS()
class SHOOTERITEMS_API UESGA_ChangeItem : public UESGameplayAbility
{
	GENERATED_BODY()

public:
	UESGA_ChangeItem();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
								const FGameplayAbilityActivationInfo ActivationInfo,
								const FGameplayEventData* TriggerEventData) override;
};
