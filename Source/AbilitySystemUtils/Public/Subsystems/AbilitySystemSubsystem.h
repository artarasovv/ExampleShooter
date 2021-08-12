// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "AbilitySystemSubsystem.generated.h"

/** Ability system setup subsystem */
UCLASS()
class ABILITYSYSTEMUTILS_API UAbilitySystemSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

	/** Called on subsystem init */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
