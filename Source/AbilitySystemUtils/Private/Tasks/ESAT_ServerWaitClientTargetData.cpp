// Copyright, Artem Tarasov, 2021


#include "Tasks/ESAT_ServerWaitClientTargetData.h"
#include "AbilitySystemComponent.h"

UESAT_ServerWaitClientTargetData* UESAT_ServerWaitClientTargetData::ServerWaitForClientTargetData(
	UGameplayAbility* OwningAbility, FName TaskInstanceName, bool TriggerOnce)
{
	UESAT_ServerWaitClientTargetData* MyObj = NewAbilityTask<UESAT_ServerWaitClientTargetData>(
		OwningAbility, TaskInstanceName);
	MyObj->bTriggerOnce = TriggerOnce;
	return MyObj;
}

void UESAT_ServerWaitClientTargetData::Activate()
{
	if (!Ability || !Ability->GetCurrentActorInfo()->IsNetAuthority())
	{
		return;
	}

	const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
	const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
	AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(
		this, &UESAT_ServerWaitClientTargetData::OnTargetDataReplicatedCallback);
}

void UESAT_ServerWaitClientTargetData::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data,
                                                                      FGameplayTag ActivationTag)
{
	const FGameplayAbilityTargetDataHandle MutableData = Data;
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(MutableData);
	}

	if (bTriggerOnce)
	{
		EndTask();
	}
}

void UESAT_ServerWaitClientTargetData::OnDestroy(bool AbilityEnded)
{
	if (AbilitySystemComponent)
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).RemoveAll(this);
	}

	Super::OnDestroy(AbilityEnded);
}
