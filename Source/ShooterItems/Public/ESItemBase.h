// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "ESItemBase.generated.h"

/** Base class for items */
UCLASS(Abstract, Blueprintable, BlueprintType)
class SHOOTERITEMS_API AESItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AESItemBase(const FObjectInitializer& ObjectInitializer);

	/** We want to disable client attachment replication to set attachment on client manually */
	virtual void OnRep_AttachmentReplication() override;

	/** Getter for SphereCollision */
	class USphereComponent* GetSphereCollision() const { return SphereCollision; }

	/** Getter for PickUpBlockingTags */
	FGameplayTagContainer GetPickUpBlockingTags () const { return PickUpBlockingTags; }
	
protected:
	/** Calls when item is picked up. Does OnPickedUp Job */
	virtual void OnPickedUp(TScriptInterface<class IAbilitySystemInterface> AbilitySystem);

	/** Calls when item is dropped. Does OnDropped Job */
	virtual void OnDropped();

	/** Gameplay effects will be applied to owner when item is picked up */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ExampleShooter | Item | Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> PickUpGameplayEffects;

	/** Gameplay abilities will be granted to owner when item is picked up */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ExampleShooter | Item | Abilities")
	TArray<TSubclassOf<class UESGameplayAbility>> PickUpGameplayAbilities;

	/** Cached owner ASC */
	UPROPERTY(BlueprintReadOnly, Category="ExampleShooter | Item | Abilities")
	TScriptInterface<class IAbilitySystemInterface> OwningAbilitySystem;

	/** Gives gameplay abilities to owning ASC. SERVER ONLY */
	UFUNCTION(BlueprintCallable)
	void GiveGameplayAbilitiesToOwner(const TArray<TSubclassOf<class UESGameplayAbility>>& GameplayAbilities,
	                                  TArray<FGameplayAbilitySpecHandle>& OutHandles);

	/** Clears gameplay abilities from owning ASC and clears array. SERVER ONLY */
	UFUNCTION(BlueprintCallable)
	void ClearGameplayAbilitiesFromOwner(TArray<FGameplayAbilitySpecHandle>& GameplayAbilities);

	/** Applies gameplay effects to owning ASC. SERVER ONLY */
	UFUNCTION(BlueprintCallable)
	void ApplyGameplayEffectsToOwner(const TArray<TSubclassOf<class UGameplayEffect>>& GameplayEffects,
	                                 TArray<FActiveGameplayEffectHandle>& OutHandles);

	/** Removes gameplay effects to owning ASC and clears array. SERVER ONLY */
	UFUNCTION(BlueprintCallable)
	void RemoveGameplayEffectsFromOwner(TArray<FActiveGameplayEffectHandle>& GameplayEffects);

	/** Turn on PickUp collision */
	UFUNCTION()
	virtual void TurnCollisionOn() const;
	
	/** Turn off PickUp collision */
	UFUNCTION()
	virtual void TurnCollisionOff() const;

public:
	/** Checks can item be picked up by AbilitySystem interface owner */
	UFUNCTION(BlueprintCallable)
	virtual bool CanBePickedUp(TScriptInterface<class IAbilitySystemInterface> AbilitySystem) const;
	
	/** Picks up item */
	UFUNCTION(BlueprintCallable)
	void PickUpItem(TScriptInterface<class IAbilitySystemInterface> AbilitySystem);

	/** Drops item */
	UFUNCTION(BlueprintCallable)
	void DropItem();

	/** Getter for owning ASC */
	UFUNCTION(BlueprintCallable)
	class UESAbilitySystemComponent* GetOwningAbilitySystemComponent() const;

private:
	/** If any is matched, then item won't pick up */
	UPROPERTY(EditDefaultsOnly, Category="ExampleShooter | Abilities")
	FGameplayTagContainer PickUpBlockingTags;
	
	/** Collision to interact with player */
	UPROPERTY(VisibleAnywhere, Category="ExampleShooter | Collision")
	class USphereComponent* SphereCollision;
	
	/** Picked up gameplay effects handles. Used to remove effects on item Drop */
	TArray<FActiveGameplayEffectHandle> PickUpActiveEffects;

	/** Picked up gameplay abilities handles. Used to remove effects on item Drop */
	TArray<FGameplayAbilitySpecHandle> PickUpActiveAbilities;
};
