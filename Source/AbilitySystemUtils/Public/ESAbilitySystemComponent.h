// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "ESAbilitySystemComponent.generated.h"

/**
* Data about montages that were played locally (all montages in case of server. predictive montages in case of client). Never replicated directly.
*/
USTRUCT()
struct ABILITYSYSTEMUTILS_API FGameplayAbilityLocalAnimMontageForMesh
{
	GENERATED_BODY()
	;

public:
	UPROPERTY()
	USkeletalMeshComponent* Mesh;

	UPROPERTY()
	FGameplayAbilityLocalAnimMontage LocalMontageInfo;

	FGameplayAbilityLocalAnimMontageForMesh() : Mesh(nullptr), LocalMontageInfo()
	{
	}

	FGameplayAbilityLocalAnimMontageForMesh(USkeletalMeshComponent* InMesh)
		: Mesh(InMesh), LocalMontageInfo()
	{
	}

	FGameplayAbilityLocalAnimMontageForMesh(USkeletalMeshComponent* InMesh,
	                                        FGameplayAbilityLocalAnimMontage& InLocalMontageInfo)
		: Mesh(InMesh), LocalMontageInfo(InLocalMontageInfo)
	{
	}
};

/**
* Data about montages that is replicated to simulated clients.
*/
USTRUCT()
struct ABILITYSYSTEMUTILS_API FGameplayAbilityRepAnimMontageForMesh
{
	GENERATED_BODY();

	public:
	UPROPERTY()
	USkeletalMeshComponent* Mesh;

	UPROPERTY()
	FGameplayAbilityRepAnimMontage RepMontageInfo;

	FGameplayAbilityRepAnimMontageForMesh() : Mesh(nullptr), RepMontageInfo()
	{
	}

	FGameplayAbilityRepAnimMontageForMesh(USkeletalMeshComponent* InMesh)
		: Mesh(InMesh), RepMontageInfo()
	{
	}
};

/** Damage delegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceivedDamageSignature, const class UESAbilitySystemComponent*,
                                               SourceASC, const float&, UnmitigatedDamage, const float&,
                                               MitigatedDamage);

/** Extended ability system component */
UCLASS()
class ABILITYSYSTEMUTILS_API UESAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	/**
	 * Grants list of abilities by ability class
	 * @param InAbilitiesArray - List of abilities to give
	 * @param Level - Ability level
	 * @param InSourceObject - Ability spec source object. Ability owner
	 * @return - Array of active abilities handles
	 */
	TArray<FGameplayAbilitySpecHandle> GiveAbilities(
		const TArray<TSubclassOf<class UESGameplayAbility>>& InAbilitiesArray, const int32& Level = 1,
		UObject* InSourceObject = nullptr);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	* Applies gameplay effect to owner ASC 
	* @param InGameplayEffect - Gameplay Effect class
	* @param Level - Gameplay effect level
	* @param InSourceObject - Effect spec source object. Effect owner
	* @return - Active gameplay effect handles
	*/
	FActiveGameplayEffectHandle ApplyGameplayEffect(const TSubclassOf<class UGameplayEffect>& InGameplayEffect,
	                                                const int32& Level = 1,
	                                                UObject* InSourceObject = nullptr);

	/**
	* Remove GE array by active handles from owner ASC
	* @param ActiveGEHandles - Array of active GE handles
	* @param StacksToRemove - Stacks of GE to remove. -1 removes all stacks
	*/
	void RemoveActiveEffectsByHandles(const TArray<FActiveGameplayEffectHandle>& ActiveGEHandles,
	                                  const int32 StacksToRemove = -1);

	/**
	* Clears given abilities by handle array
	* @param AbilitiesHandles - Array of GA handles
	*/
	void ClearAbilities(const TArray<FGameplayAbilitySpecHandle>& AbilitiesHandles);

	/**
	* Applies list of gameplay effect to owner ASC 
	* @param InGameplayEffects - List of Gameplay Effect class
	* @param Level - Gameplay effect level
	* @param InSourceObject - Effect spec source object. Effect owner
	* @return - Array of active GE handles
	*/
	TArray<FActiveGameplayEffectHandle> ApplyGameplayEffects(
		const TArray<TSubclassOf<class UGameplayEffect>>& InGameplayEffects,
		const int32& Level = 1, UObject* InSourceObject = nullptr);

	/** Removes gameplay effects from ASC by gameplay tag */
	void RemoveGameplayEffectsByTag(const FGameplayTag& Tag);

	/** Called from ESDamageExecCalculation. Broadcasts on ReceivedDamage whenever this ASC receives damage */
	virtual void ReceiveDamage(const UESAbilitySystemComponent* SourceAsc, const float& UnmitigatedDamage,
	                           const float& MitigatedDamage);

	/** Received Damage delegate */
	UPROPERTY(BlueprintAssignable)
	FReceivedDamageSignature ReceivedDamage;

	/** Attempts to activate the given ability handle and batch all RPCs into one. This will only batch all RPCs that happen
	 * in one frame. Best case scenario we batch ActivateAbility, SendTargetData, and EndAbility into one RPC instead of three.
	 * Worst case we batch ActivateAbility and SendTargetData into one RPC instead of two and call EndAbility later in a separate
	 * RPC. If we can't batch SendTargetData or EndAbility with ActivateAbility because they don't happen in the same frame due to
	 * latent ability tasks for example, then batching doesn't help and we should just activate normally.
	 * Single shots (semi auto fire) combine ActivateAbility, SendTargetData, and EndAbility into one RPC instead of three.
	 * Full auto shots combine ActivateAbility and SendTargetData into one RPC instead of two for the first bullet. Each subsequent
	 * bullet is one RPC for SendTargetData. We then send one final RPC for the EndAbility when we're done firing.
	 */
	UFUNCTION(BlueprintCallable, Category = "ExampleShooter | Abilities")
	virtual bool BatchRPCTryActivateAbility(FGameplayAbilitySpecHandle InAbilityHandle, bool EndAbilityImmediately);

	// Returns the current animating ability
	UGameplayAbility* GetAnimatingAbilityFromAnyMesh();

	// Returns the montage that is playing for the mesh
	UAnimMontage* GetCurrentMontageForMesh(USkeletalMeshComponent* InMesh);

	// Stops whatever montage is currently playing. Expectation is caller should only be stopping it if they are the current animating ability (or have good reason not to check)
	virtual void CurrentMontageStopForMesh(USkeletalMeshComponent* InMesh, float OverrideBlendOutTime = -1.0f);

	// Data structure for replicating montage info to simulated clients
	// Will be max one element per skeletal mesh on the AvatarActor
	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedAnimMontageForMesh)
	TArray<FGameplayAbilityRepAnimMontageForMesh> RepAnimMontageInfoForMeshes;
	
	// Finds the existing FGameplayAbilityRepAnimMontageForMesh for the mesh or creates one if it doesn't exist
	FGameplayAbilityRepAnimMontageForMesh& GetGameplayAbilityRepAnimMontageForMesh(USkeletalMeshComponent* InMesh);
	
	// Copy LocalAnimMontageInfo into RepAnimMontageInfo
	void AnimMontage_UpdateReplicatedDataForMesh(USkeletalMeshComponent* InMesh);
	void AnimMontage_UpdateReplicatedDataForMesh(FGameplayAbilityRepAnimMontageForMesh& OutRepAnimMontageInfo);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities")
	FGameplayAbilitySpecHandle FindAbilitySpecHandleForClass(TSubclassOf<UGameplayAbility> AbilityClass,
	                                                         UObject* OptionalSourceObject = nullptr);

	// Plays a montage and handles replication and prediction based on passed in ability/activation info
	virtual float PlayMontageForMesh(UGameplayAbility* AnimatingAbility, class USkeletalMeshComponent* InMesh,
	                                 FGameplayAbilityActivationInfo ActivationInfo, UAnimMontage* Montage,
	                                 float InPlayRate, FName StartSectionName = NAME_None,
	                                 bool bReplicateMontage = true);

	// Plays a montage without updating replication/prediction structures. Used by simulated proxies when replication tells them to play a montage.
	virtual float PlayMontageSimulatedForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None);


protected:
	// Called when a prediction key that played a montage is rejected
	void OnPredictiveMontageRejectedForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* PredictiveMontage);
	
	// Returns true if we are ready to handle replicated montage information
	virtual bool IsReadyForReplicatedMontageForMesh();
	
	// Data structure for montages that were instigated locally (everything if server, predictive if client. replicated if simulated proxy)
	// Will be max one element per skeletal mesh on the AvatarActor
	UPROPERTY()
	TArray<FGameplayAbilityLocalAnimMontageForMesh> LocalAnimMontageInfoForMeshes;

	// Finds the existing FGameplayAbilityLocalAnimMontageForMesh for the mesh or creates one if it doesn't exist
	FGameplayAbilityLocalAnimMontageForMesh& GetLocalAnimMontageInfoForMesh(USkeletalMeshComponent* InMesh);

	UFUNCTION()
	virtual void OnRep_ReplicatedAnimMontageForMesh();
};
