// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "ESAT_WaitReceiveDamage.generated.h"

/** Declare Wait receive damage delegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWaitReceiveDamageSignature, const class UESAbilitySystemComponent*,
                                               SourceASC, const float&, UnmitigatedDamage, const float&,
                                               MitigatedDamage);

/** Waits until the Ability Owner receives damage */
UCLASS()
class ABILITYSYSTEMUTILS_API UESAT_WaitReceiveDamage : public UAbilityTask
{
	GENERATED_BODY()

public:
	UESAT_WaitReceiveDamage(const FObjectInitializer& ObjectInitializer);

	/** Damage receive delegate */
	UPROPERTY(BlueprintAssignable)
	FWaitReceiveDamageSignature OnDamage;

	/** Ability task activation function */
	virtual void Activate() override;

	/** Called when damage received */
	UFUNCTION()
	void OnDamageReceived(const class UESAbilitySystemComponent* SourceASC, const float& UnmitigatedDamage,
	                      const float& MitigatedDamage);

	/** Wait until the ability owner receives damage */
	UFUNCTION(BlueprintCallable, Category = "ExampleShooter | Ability|Tasks",
		meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UESAT_WaitReceiveDamage* WaitReceiveDamage(UGameplayAbility* OwningAbility, bool bTriggerOnce);

protected:
	bool bTriggerOnce;

	virtual void OnDestroy(bool AbilityIsEnding) override;
};
