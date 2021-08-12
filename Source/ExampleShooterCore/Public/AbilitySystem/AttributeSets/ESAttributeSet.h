// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ESAttributeSet.generated.h"


/** Generates getters and setters for attributes */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/** Class for all common gameplay attributes */
UCLASS()
class EXAMPLESHOOTERCORE_API UESAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	/** Called before attribute change */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/** Called after gameplay effect executing */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	/** Properties network replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * Current Health, when 0 we expect owner to die unless prevented by an ability. Capped by MaxHealth.
	 * Positive changes can directly use this.
	 * Negative changes to Health should go through Damage meta attribute.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "ExampleShooter | Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UESAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "ExampleShooter | Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UESAttributeSet, MaxHealth)

	/**
	 * Damage is a meta attribute used by the DamageExecution to calculate final damage, which then turns into -Health
	 * Temporary value that only exists on the Server. Not replicated.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "ExampleShooter | Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UESAttributeSet, Damage)

protected:
	/** Called on replication */
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	/** Called on replication */
	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	/** Called on Max attribute change. Makes percent affected attribute change. */
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,
	                                 const FGameplayAttributeData& MaxAttribute, const float& NewMaxValue,
	                                 const FGameplayAttribute& AffectedAttributeProperty) const;
};
