// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "ESAmmoAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/** Reserve ammo attribute set. Attribute set stores only reserve ammo */
UCLASS()
class SHOOTERCOMBAT_API UESAmmoAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UESAmmoAttributeSet();

	UPROPERTY(BlueprintReadOnly, Category = "ExampleShooter | Ammo", ReplicatedUsing = OnRep_RifleReserveAmmo)
	FGameplayAttributeData RifleReserveAmmo;
	ATTRIBUTE_ACCESSORS(UESAmmoAttributeSet, RifleReserveAmmo)

	UPROPERTY(BlueprintReadOnly, Category = "ExampleShooter | Ammo", ReplicatedUsing = OnRep_MaxRifleReserveAmmo)
	FGameplayAttributeData MaxRifleReserveAmmo;
	ATTRIBUTE_ACCESSORS(UESAmmoAttributeSet, MaxRifleReserveAmmo)

	UPROPERTY(BlueprintReadOnly, Category = "ExampleShooter | Ammo", ReplicatedUsing = OnRep_SniperReserveAmmo)
	FGameplayAttributeData SniperReserveAmmo;
	ATTRIBUTE_ACCESSORS(UESAmmoAttributeSet, SniperReserveAmmo)

	UPROPERTY(BlueprintReadOnly, Category = "ExampleShooter | Ammo", ReplicatedUsing = OnRep_MaxSniperReserveAmmo)
	FGameplayAttributeData MaxSniperReserveAmmo;
	ATTRIBUTE_ACCESSORS(UESAmmoAttributeSet, MaxSniperReserveAmmo)

	UPROPERTY(BlueprintReadOnly, Category = "ExampleShooter | Ammo", ReplicatedUsing = OnRep_ShotgunReserveAmmo)
	FGameplayAttributeData ShotgunReserveAmmo;
	ATTRIBUTE_ACCESSORS(UESAmmoAttributeSet, ShotgunReserveAmmo)

	UPROPERTY(BlueprintReadOnly, Category = "ExampleShooter | Ammo", ReplicatedUsing = OnRep_MaxShotgunReserveAmmo)
	FGameplayAttributeData MaxShotgunReserveAmmo;
	ATTRIBUTE_ACCESSORS(UESAmmoAttributeSet, MaxShotgunReserveAmmo)

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	static FGameplayAttribute GetReserveAmmoAttributeFromTag(const FGameplayTag& PrimaryAmmoTag);
	static FGameplayAttribute GetMaxReserveAmmoAttributeFromTag(const FGameplayTag& PrimaryAmmoTag);

protected:
	// Cache tags
	FGameplayTag RifleAmmoTag;
	FGameplayTag SniperAmmoTag;
	FGameplayTag ShotgunAmmoTag;

	// Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes.
	// (i.e. When MaxHealth increases, Health increases by an amount that maintains the same percentage as before)
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const;

	/**
	* These OnRep functions exist to make sure that the ability system internal representations are synchronized properly during replication
	**/
	
	UFUNCTION()
	virtual void OnRep_RifleReserveAmmo(const FGameplayAttributeData& OldRifleReserveAmmo);

	UFUNCTION()
	virtual void OnRep_MaxRifleReserveAmmo(const FGameplayAttributeData& OldMaxRifleReserveAmmo);

	UFUNCTION()
	virtual void OnRep_SniperReserveAmmo(const FGameplayAttributeData& OldSniperReserveAmmo);

	UFUNCTION()
	virtual void OnRep_MaxSniperReserveAmmo(const FGameplayAttributeData& OldMaxSniperReserveAmmo);

	UFUNCTION()
	virtual void OnRep_ShotgunReserveAmmo(const FGameplayAttributeData& OldShotgunReserveAmmo);

	UFUNCTION()
	virtual void OnRep_MaxShotgunReserveAmmo(const FGameplayAttributeData& OldMaxShotgunReserveAmmo);
};
