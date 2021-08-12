// Copyright, Artem Tarasov, 2021


#include "Tasks/ESAT_WaitReceiveDamage.h"
#include "ESAbilitySystemComponent.h"

UESAT_WaitReceiveDamage::UESAT_WaitReceiveDamage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTriggerOnce = false;
}

UESAT_WaitReceiveDamage* UESAT_WaitReceiveDamage::WaitReceiveDamage(UGameplayAbility* OwningAbility, bool bTriggerOnce)
{
	UESAT_WaitReceiveDamage* MyObj = NewAbilityTask<UESAT_WaitReceiveDamage>(OwningAbility);
	MyObj->bTriggerOnce = bTriggerOnce;
	return MyObj;
}

void UESAT_WaitReceiveDamage::Activate()
{
	UESAbilitySystemComponent* ASC = Cast<UESAbilitySystemComponent>(AbilitySystemComponent);

	if (ASC)
	{
		ASC->ReceivedDamage.AddDynamic(this, &UESAT_WaitReceiveDamage::OnDamageReceived);
	}
}

void UESAT_WaitReceiveDamage::OnDamageReceived(const UESAbilitySystemComponent* SourceASC,
                                               const float& UnmitigatedDamage, const float& MitigatedDamage)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
	}

	if (bTriggerOnce)
	{
		EndTask();
	}
}

void UESAT_WaitReceiveDamage::OnDestroy(bool AbilityIsEnding)
{
	UESAbilitySystemComponent* ASC = Cast<UESAbilitySystemComponent>(AbilitySystemComponent);

	if (ASC)
	{
		ASC->ReceivedDamage.RemoveDynamic(this, &UESAT_WaitReceiveDamage::OnDamageReceived);
	}

	Super::OnDestroy(AbilityIsEnding);
}
