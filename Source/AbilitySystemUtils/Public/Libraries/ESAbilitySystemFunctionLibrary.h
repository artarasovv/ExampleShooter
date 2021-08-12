// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Abilities/ESGameplayAbility.h"
#include "ESAbilitySystemFunctionLibrary.generated.h"

/** Extension of gameplay ability system blueprint library */
UCLASS()
class ABILITYSYSTEMUTILS_API UESAbilitySystemFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ExampleShooter | Ability")
	static bool IsAbilitySpecHandleValid(FGameplayAbilitySpecHandle Handle);

	UFUNCTION(BlueprintCallable, Category = "ExampleShooter | Ability")
	static UESGameplayAbility* GetPrimaryAbilityInstanceFromHandle(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle);

	UFUNCTION(BlueprintCallable, Category = "ExampleShooter | Ability")
	static bool IsPrimaryAbilityInstanceActive(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle);

	// Adds TargetData
	UFUNCTION(BlueprintCallable, Category = "Ability|EffectContext", Meta = (DisplayName = "AddTargetData"))
	static void EffectContextAddTargetData(FGameplayEffectContextHandle EffectContextHandle, const FGameplayAbilityTargetDataHandle& TargetData);
};
