// Copyright, Artem Tarasov, 2021


#include "Characters/Pawns/ESCharacter.h"
#include "ESAbilitySystemComponent.h"
#include "ESEquipableItemBase.h"
#include "Abilities/ESGameplayAbility.h"
#include "AbilitySystem/AttributeSets/ESAmmoAttributeSet.h"
#include "AbilitySystem/AttributeSets/ESAttributeSet.h"
#include "Camera/CameraComponent.h"
#include "Characters/Controllers/ESPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Inventory/ESInventoryComponent.h"
#include "Inventory/InventorySlot.h"
#include "Items/ESWeaponBase.h"
#include "Net/UnrealNetwork.h"

AESCharacter::AESCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UESAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->ReplicationMode = EGameplayEffectReplicationMode::Mixed;

	AttributeSet = CreateDefaultSubobject<UESAttributeSet>(TEXT("AttributeSet"));
	AmmoAttributeSet = CreateDefaultSubobject<UESAmmoAttributeSet>(TEXT("AmmoAttributeSet"));

	InventoryComponent = CreateDefaultSubobject<UESInventoryComponent>(TEXT("Inventory"));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(GetRootComponent());
	CameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
	CameraComponent->bUsePawnControlRotation = true;

	ArmsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmsMesh"));
	ArmsMesh->SetupAttachment(CameraComponent);
	ArmsMesh->SetRelativeLocation(FVector(6.f, 0.f, -155.f));
	ArmsMesh->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	ArmsMesh->SetOnlyOwnerSee(true);

	GetCapsuleComponent()->SetCapsuleHalfHeight(96.f);
	GetCapsuleComponent()->SetCapsuleRadius(42.f);

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetOwnerNoSee(true);

	GetCharacterMovement()->MaxWalkSpeed = 700.f;
	GetCharacterMovement()->BrakingFrictionFactor = 3.f;
}

UAbilitySystemComponent* AESCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UESInventoryComponent* AESCharacter::GetInventoryComponent_Implementation() const
{
	return InventoryComponent;
}

bool AESCharacter::GetWeaponMuzzleTransform_Implementation(FTransform& OutTransform) const
{
	if (!CurrentWeapon)
		return false;

	OutTransform = CurrentWeapon->GetSkeletalMesh()->GetSocketTransform(FName("Muzzle"));
	OutTransform.SetScale3D(FVector(1.f));

	return true;
}

USkeletalMeshComponent* AESCharacter::GetNetRelevantMesh_Implementation() const
{
	return IsLocallyControlled() ? GetArmsMesh() : GetMesh();
}

void AESCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	if (AESPlayerController* ESPlayerController = Cast<AESPlayerController>(NewController))
	{
		//chache interfaces on server
		ESPlayerController->PawnASCInterface.SetObject(this);
		ESPlayerController->PawnASCInterface.SetInterface(Cast<IAbilitySystemInterface>(this));
	}

	SetOwner(NewController);
	ClientBindASCInput();
}

void AESCharacter::ClientBindASCInput_Implementation() const
{
	if (!GetAbilitySystemComponent())
		return;

	GetAbilitySystemComponent()->BindAbilityActivationToInputComponent(
		GetController()->InputComponent,
		FGameplayAbilityInputBinds(FString("Confirm"), FString("Cancel"), FString("EESAbilityInputID"),
								static_cast<int32>(EESAbilityInputID::Confirm),
								static_cast<int32>(EESAbilityInputID::Cancel)));
}

void AESCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AESCharacter, CurrentWeapon);
}

void AESCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority && AbilitySystemComponent)
	{
		AbilitySystemComponent->GiveAbilities(StartupAbilities);
		AbilitySystemComponent->ApplyGameplayEffect(DefaultAttributes, 1, this);
		AbilitySystemComponent->ApplyGameplayEffects(StartupEffects, 1, this);

		InventoryComponent->OnSlotChanged.AddDynamic(this, &AESCharacter::OnInventorySlotChanged);
	}

	if (GetLocalRole() == ROLE_SimulatedProxy && InventoryComponent) // We don't need inventory on simulated proxies
	{
		InventoryComponent->DestroyComponent();
		InventoryComponent = nullptr;
	}
}

void AESCharacter::OnRep_CurrentWeapon(AESWeaponBase* OldCurrentWeapon)
{
	OnWeaponChanged.Broadcast(CurrentWeapon);
}

void AESCharacter::OnInventorySlotChanged()
{
	AESWeaponBase* NewWeapon = nullptr;
	if (InventoryComponent->GetActiveItem())
		NewWeapon = Cast<AESWeaponBase>(InventoryComponent->GetActiveItem());

	CurrentWeapon = NewWeapon;
	OnWeaponChanged.Broadcast(NewWeapon);
}
