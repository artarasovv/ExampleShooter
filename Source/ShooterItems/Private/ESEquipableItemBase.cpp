// Copyright, Artem Tarasov, 2021


#include "ESEquipableItemBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Interfaces/ItemAttachInterface.h"
#include "Kismet/KismetMathLibrary.h"

AESEquipableItemBase::AESEquipableItemBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetReplicatingMovement(true);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetGenerateOverlapEvents(false);
	SkeletalMesh->CanCharacterStepUpOn = ECB_No;
	SetRootComponent(SkeletalMesh);

	AESEquipableItemBase::TurnCollisionOn();
	SkeletalMesh->CanCharacterStepUpOn = ECB_No;
	SkeletalMesh->SetSimulatePhysics(true);

	GetSphereCollision()->SetupAttachment(SkeletalMesh);
	
	DropImpulseMagnitude = 300.f;
}

void AESEquipableItemBase::OnPickedUp(TScriptInterface<IAbilitySystemInterface> AbilitySystem)
{
	Super::OnPickedUp(AbilitySystem);

	if (HasAuthority())
	{
		MultiAttachOnPickUp(OwningAbilitySystem.GetObject());
	}
}

void AESEquipableItemBase::OnDropped()
{
	Super::OnDropped();

	if (HasAuthority())
	{
		MultiDetachOnDrop(OwningAbilitySystem.GetObject());
	}
}

void AESEquipableItemBase::OnEquipped()
{
	//Apply gameplay effects and grant abilities
	GiveGameplayAbilitiesToOwner(EquippedGameplayAbilities, EquipActiveAbilities);
	ApplyGameplayEffectsToOwner(EquippedGameplayEffects, EquipActiveEffects);
}

void AESEquipableItemBase::OnUnEquipped()
{
	//Remove effects and abilities
	ClearGameplayAbilitiesFromOwner(EquipActiveAbilities);
	RemoveGameplayEffectsFromOwner(EquipActiveEffects);
}

void AESEquipableItemBase::EquipItem()
{
	OnEquipped();
}

void AESEquipableItemBase::UnEquipItem()
{
	OnUnEquipped();
}

void AESEquipableItemBase::MultiDetachOnDrop_Implementation(UObject* ASCOwner)
{
	SetReplicatingMovement(true);

	TurnCollisionOn();
	SkeletalMesh->SetSimulatePhysics(true);

	//Turn on sphere a bit later
	Super::TurnCollisionOff();
	FTimerHandle Handle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &AESEquipableItemBase::TurnCollisionOn);
	GetWorld()->GetTimerManager().SetTimer(Handle, TimerDelegate, 1.5f, false);

	if (ACharacter* Character = Cast<ACharacter>(ASCOwner))
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		if (HasAuthority())
		{
			SetActorLocation(
				Character->GetActorLocation() + Character->GetActorForwardVector() * 100.f + FVector(0.f, 0.f, 60.f),
				false, nullptr, ETeleportType::TeleportPhysics);
			GetSkeletalMesh()->SetAllPhysicsLinearVelocity(
				Character->GetMovementComponent()->Velocity * .6f);
			GetSkeletalMesh()->AddImpulse(
				UKismetMathLibrary::GetForwardVector(Character->GetControlRotation()) * DropImpulseMagnitude,
				NAME_None,
				true); //Drop item with impulse
		}
	}
}

void AESEquipableItemBase::MultiAttachOnPickUp_Implementation(UObject* ASCOwner)
{
	SetReplicatingMovement(false);

	SkeletalMesh->SetSimulatePhysics(false);
	TurnCollisionOff();

	if (ACharacter* Character = Cast<ACharacter>(ASCOwner))
	{
		USkeletalMeshComponent* TargetMesh = Character->GetMesh();
		if (Character->GetClass()->ImplementsInterface(UItemAttachInterface::StaticClass()))
		{
			TargetMesh = IItemAttachInterface::Execute_GetNetRelevantMesh(Character);
		}

		AttachToComponent(TargetMesh,
		                  FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		                  GetAttachBone());
		const FTransform AttachTransform = Character->IsLocallyControlled()
			                                   ? GetArmsAttachTransform()
			                                   : GetBodyAttachTransform();
		SetActorRelativeTransform(AttachTransform);
	}
}

void AESEquipableItemBase::TurnCollisionOn() const
{
	Super::TurnCollisionOn();
	
	SkeletalMesh->SetCollisionObjectType(ECC_WorldDynamic);
	SkeletalMesh->SetCollisionResponseToAllChannels(ECR_Block);
	SkeletalMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
}

void AESEquipableItemBase::TurnCollisionOff() const
{
	Super::TurnCollisionOff();
	
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	SkeletalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
}
