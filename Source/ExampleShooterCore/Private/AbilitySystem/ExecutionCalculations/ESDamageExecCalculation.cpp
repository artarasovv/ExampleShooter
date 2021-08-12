// Copyright, Artem Tarasov, 2021


#include "AbilitySystem/ExecutionCalculations/ESDamageExecCalculation.h"

#include "ESAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/ESAttributeSet.h"

// Declare the attributes to capture and define how we want to capture them from the Source and Target.
struct ESDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);

	ESDamageStatics()
	{
		// Snapshot happens at time of GESpec creation

		// Capture optional Damage set on the damage GE as a CalculationModifier under the ExecutionCalculation
		DEFINE_ATTRIBUTE_CAPTUREDEF(UESAttributeSet, Damage, Source, true);
	}
};

static const ESDamageStatics& DamageStatics()
{
	static ESDamageStatics DStatics;
	return DStatics;
}

UESDamageExecCalculation::UESDamageExecCalculation()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
}

void UESDamageExecCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                      FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Damage = 0.0f;
	// Capture optional damage value set on the damage GE as a CalculationModifier under the ExecutionCalculation
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters,
		Damage);
	// Add SetByCaller damage if it exists
	Damage += FMath::Max<float>(Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")),
		false, -1.0f), 0.0f);

	const float UnmitigatedDamage = Damage; // Can multiply any damage boosters here
	const float MitigatedDamage = UnmitigatedDamage;

	if (MitigatedDamage > 0.f)
	{
		// Set the Target's damage meta attribute
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty,
			EGameplayModOp::Additive, MitigatedDamage));
	}

	// Broadcast damages to Target ASC
	UESAbilitySystemComponent* TargetASC = Cast<UESAbilitySystemComponent>(TargetAbilitySystemComponent);
	if (TargetASC)
	{
		UESAbilitySystemComponent* SourceASC = Cast<UESAbilitySystemComponent>(SourceAbilitySystemComponent);
		TargetASC->ReceiveDamage(SourceASC, UnmitigatedDamage, MitigatedDamage);
	}
}