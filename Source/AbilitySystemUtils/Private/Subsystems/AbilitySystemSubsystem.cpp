// Copyright, Artem Tarasov, 2021


#include "Subsystems/AbilitySystemSubsystem.h"
#include "AbilitySystemGlobals.h"


void UAbilitySystemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UAbilitySystemGlobals::Get().InitGlobalData();
}
