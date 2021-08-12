// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ESDamageExecCalculation.generated.h"

/** Advanced damage calculation class. Can calculate damage mitigations, armors and more if needed */
UCLASS()
class EXAMPLESHOOTERCORE_API UESDamageExecCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UESDamageExecCalculation();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
