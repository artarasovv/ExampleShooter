// Copyright, Artem Tarasov, 2021


#include "Tasks/ESAT_WaitDelayOneFrame.h"
#include "TimerManager.h"

void UESAT_WaitDelayOneFrame::Activate()
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UESAT_WaitDelayOneFrame::OnDelayFinish);
}

UESAT_WaitDelayOneFrame* UESAT_WaitDelayOneFrame::WaitDelayOneFrame(UGameplayAbility* OwningAbility)
{
	UESAT_WaitDelayOneFrame* MyObj = NewAbilityTask<UESAT_WaitDelayOneFrame>(OwningAbility);
	return MyObj;
}

void UESAT_WaitDelayOneFrame::OnDelayFinish()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnFinish.Broadcast();
	}
	EndTask();
}