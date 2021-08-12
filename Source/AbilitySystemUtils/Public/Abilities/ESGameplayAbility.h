// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ESGameplayAbility.generated.h"

/** Enum to directly bind abilities activation on user input */
UENUM(BlueprintType)
enum class EESAbilityInputID : uint8
{
	None UMETA(DisplayName = "None"),
	Confirm UMETA(DisplayName = "Confirm"),
	Cancel UMETA(DisplayName = "Cancel"),
	Fire UMETA(DisplayName = "Fire"),
	Reload UMETA(DisplayName = "Reload"),
	DropItem UMETA(DisplayName = "DropItem"),
	Invisibility UMETA(DisplayName = "Invisibility")
};

USTRUCT()
struct ABILITYSYSTEMUTILS_API FAbilityMeshMontage
{
	GENERATED_BODY()

	public:
	UPROPERTY()
	class USkeletalMeshComponent* Mesh;

	UPROPERTY()
	class UAnimMontage* Montage;

	FAbilityMeshMontage() : Mesh(nullptr), Montage(nullptr)
	{
	}

	FAbilityMeshMontage(class USkeletalMeshComponent* InMesh, class UAnimMontage* InMontage) 
		: Mesh(InMesh), Montage(InMontage)
	{
	}
};

/** Extended gameplay ability class */
UCLASS()
class ABILITYSYSTEMUTILS_API UESGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UESGameplayAbility();

	/** Abilities with this set will automatically activate when the input is pressed */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ExampleShooter | Ability")
	EESAbilityInputID AbilityInputID = EESAbilityInputID::None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ExampleShooter | Ability")
	bool bActivateAbilityOnGranted = false;

	/** Call to set/get the current montage from a montage task. Set to allow hooking up montage events to ability events */
	virtual void SetCurrentMontageForMesh(USkeletalMeshComponent* InMesh, class UAnimMontage* InCurrentMontage);
	
	/** Expose GetSourceObject to Blueprint. Retrieves the SourceObject associated with this ability. Callable on non instanced abilities. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability", meta = (DisplayName = "Get Source Object"))
	UObject* K2_GetSourceObject(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const;
	
	/** Called after ability granted to ability avatar */
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	/** Same as calling K2_EndAbility. Meant for use with batching system to end the ability externally. */
	virtual void ExternalEndAbility();

	/** Attempts to activate the given ability handle and batch all RPCs into one. This will only batch all RPCs that happen
	 * in one frame. Best case scenario we batch ActivateAbility, SendTargetData, and EndAbility into one RPC instead of three.
	 */
	UFUNCTION(BlueprintCallable, Category = "ExampleShooter | Ability")
	virtual bool BatchRPCTryActivateAbility(FGameplayAbilitySpecHandle InAbilityHandle, bool EndAbilityImmediately);

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	/** Allows C++ and Blueprint abilities to override how cost is checked in case they don't use a GE like weapon ammo */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ExampleShooter | ExampleShooter | Ability")
	bool ESCheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const;
	virtual bool ESCheckCost_Implementation(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const;

	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	/** Allows C++ and Blueprint abilities to override how cost is applied in case they don't use a GE like weapon ammo */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability")
	void ESApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const;
	virtual void ESApplyCost_Implementation(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const {}

protected:
	// ----------------------------------------------------------------------------------------------------------------
	//	Animation Support for multiple USkeletalMeshComponents on the AvatarActor
	// ----------------------------------------------------------------------------------------------------------------

	/** Active montages being played by this ability */
	UPROPERTY()
	TArray<FAbilityMeshMontage> CurrentAbilityMeshMontages;
	
	bool FindAbillityMeshMontage(USkeletalMeshComponent* InMesh, FAbilityMeshMontage& InAbilityMontage);
};
