// Copyright, Artem Tarasov, 2021


#include "ESItemBase.h"
#include "AbilitySystemInterface.h"
#include "ESAbilitySystemComponent.h"
#include "Abilities/ESGameplayAbility.h"
#include "Components/SphereComponent.h"

AESItemBase::AESItemBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(SphereCollision);
	SphereCollision->SetSphereRadius(30.f);

	AESItemBase::TurnCollisionOn();

	PickUpBlockingTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Dead")));
	PickUpBlockingTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Invisible")));
}

void AESItemBase::OnRep_AttachmentReplication()
{
	//Leave this empty in order to disable client call of attachment replication
}

void AESItemBase::OnPickedUp(TScriptInterface<IAbilitySystemInterface> AbilitySystem)
{	
	//Apply gameplay effects and grant abilities
	GiveGameplayAbilitiesToOwner(PickUpGameplayAbilities, PickUpActiveAbilities);
	ApplyGameplayEffectsToOwner(PickUpGameplayEffects, PickUpActiveEffects);
}

void AESItemBase::OnDropped()
{
	//Remove effects and abilities
	ClearGameplayAbilitiesFromOwner(PickUpActiveAbilities);
	RemoveGameplayEffectsFromOwner(PickUpActiveEffects);
}

void AESItemBase::GiveGameplayAbilitiesToOwner(const TArray<TSubclassOf<class UESGameplayAbility>>& GameplayAbilities,
                                             TArray<FGameplayAbilitySpecHandle>& OutHandles)
{
	if (GetLocalRole() != ROLE_Authority)
		return;
	
	UESAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();

	if (!ASC)
		return;

	OutHandles = ASC->GiveAbilities(GameplayAbilities, 1, this);
}

void AESItemBase::ClearGameplayAbilitiesFromOwner(TArray<FGameplayAbilitySpecHandle>& GameplayAbilities)
{
	if (GetLocalRole() != ROLE_Authority)
		return;
	
	UESAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();

	if (!ASC)
		return;

	ASC->ClearAbilities(GameplayAbilities);
	GameplayAbilities.Empty();
}

void AESItemBase::ApplyGameplayEffectsToOwner(const TArray<TSubclassOf<UGameplayEffect>>& GameplayEffects,
                                            TArray<FActiveGameplayEffectHandle>& OutHandles)
{
	if (GetLocalRole() != ROLE_Authority)
		return;
	
	UESAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();

	if (!ASC)
		return;

	OutHandles = ASC->ApplyGameplayEffects(GameplayEffects, 1, this);
}

void AESItemBase::RemoveGameplayEffectsFromOwner(TArray<FActiveGameplayEffectHandle>& GameplayEffects)
{
	if (GetLocalRole() != ROLE_Authority)
		return;
	
	UESAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();

	if (!ASC)
		return;

	ASC->RemoveActiveEffectsByHandles(GameplayEffects);
	GameplayEffects.Empty();
}

void AESItemBase::TurnCollisionOn() const
{
	SphereCollision->SetCollisionObjectType(ECC_WorldDynamic);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Overlap);
}

void AESItemBase::TurnCollisionOff() const
{
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	SphereCollision->SetGenerateOverlapEvents(false);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
}

bool AESItemBase::CanBePickedUp(TScriptInterface<IAbilitySystemInterface> AbilitySystem) const
{
	if (!AbilitySystem)
		return false;

	if (OwningAbilitySystem)
		return false;

	FGameplayTagContainer OutTagContainer;
	AbilitySystem->GetAbilitySystemComponent()->GetOwnedGameplayTags(OutTagContainer);

	const FGameplayTagQuery BlockingTagsQuery = FGameplayTagQuery::MakeQuery_MatchAnyTags(GetPickUpBlockingTags());
	return !BlockingTagsQuery.Matches(OutTagContainer);
}

void AESItemBase::PickUpItem(TScriptInterface<IAbilitySystemInterface> AbilitySystem)
{
	if (!AbilitySystem)
		return;

	OwningAbilitySystem = AbilitySystem;
	OnPickedUp(AbilitySystem);
}

void AESItemBase::DropItem()
{
	OnDropped();
	OwningAbilitySystem = nullptr;
}

UESAbilitySystemComponent* AESItemBase::GetOwningAbilitySystemComponent() const
{
	if (!OwningAbilitySystem)
		return nullptr;

	UAbilitySystemComponent* ASC = OwningAbilitySystem->GetAbilitySystemComponent();

	if (!ASC)
		return nullptr;

	return Cast<UESAbilitySystemComponent>(ASC);
}