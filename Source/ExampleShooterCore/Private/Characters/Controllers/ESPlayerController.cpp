// Copyright, Artem Tarasov, 2021


#include "Characters/Controllers/ESPlayerController.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "ESItemBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/ESGameplayAbility.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Inventory/ESInventoryComponent.h"
#include "Kismet/KismetMathLibrary.h"

AESPlayerController::AESPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AESPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	if (P)
	{
		if (P->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass()))
		{
			IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(P);
			if (AbilitySystemInterface)
			{
				//initializing abilities on client
				AbilitySystemInterface->GetAbilitySystemComponent()->InitAbilityActorInfo(P, P);

				//cache interface on client
				PawnASCInterface.SetObject(P);
				PawnASCInterface.SetInterface(AbilitySystemInterface);
			}
		}
	}
}

void AESPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis(FName(TEXT("MoveForward")), this, &AESPlayerController::OnMoveForwardInput);
	InputComponent->BindAxis(FName(TEXT("MoveRight")), this, &AESPlayerController::OnMoveRightInput);
	InputComponent->BindAxis(FName(TEXT("Pitch")), this, &AESPlayerController::OnPitchInput);
	InputComponent->BindAxis(FName(TEXT("Yaw")), this, &AESPlayerController::OnYawInput);

	InputComponent->BindAction(FName(TEXT("ToggleCursor")), IE_Pressed, this,
	                           &AESPlayerController::ToggleCursor);
	InputComponent->BindAction(FName(TEXT("NextItem")), IE_Pressed, this, &AESPlayerController::OnNextItemInput);
	InputComponent->BindAction(FName(TEXT("PreviousItem")), IE_Pressed, this,
	                           &AESPlayerController::OnPreviousItemInput);
	InputComponent->BindAction(FName(TEXT("Invisibility")), IE_Pressed, this, &AESPlayerController::OnInvisilityInput);
}

void AESPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AESPlayerController::TryInitiateServerTravel_Implementation(const FString& InURL, bool bAbsolute,
                                                                 bool bShouldSkipGameNotify)
{
	if (CanPlayerInitiateServerTravel())
	{
		GetWorld()->ServerTravel(InURL, bAbsolute, bShouldSkipGameNotify);
	}
}

void AESPlayerController::TrySpawnItem_Implementation(TSubclassOf<AESItemBase> ItemClass)
{
	if (ItemClass.Get() && CanPlayerSpawnItem() && GetPawnOrSpectator())
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		const FVector ItemLocation = GetPawnOrSpectator()->GetActorLocation() + UKismetMathLibrary::GetForwardVector(
			GetPawnOrSpectator()->GetControlRotation()) * 150.f;

		const FRotator ItemRotation = FRotator::ZeroRotator;

		GetWorld()->SpawnActor(ItemClass, &ItemLocation, &ItemRotation, SpawnParameters);
	}
}

bool AESPlayerController::CanPlayerInitiateServerTravel() const
{
	if (!HasAuthority()) //Execute check only on server
		return false;

	//Add your logic here
	return true;
}

bool AESPlayerController::CanPlayerSpawnItem() const
{
	if (!HasAuthority()) //Execute check only on server
		return false;

	return true;
}

void AESPlayerController::OnMoveForwardInput(float AxisValue)
{
	if (AxisValue == 0.f || !GetCharacter())
		return;

	GetCharacter()->AddMovementInput(GetCharacter()->GetActorForwardVector(), AxisValue);
}

void AESPlayerController::OnMoveRightInput(float AxisValue)
{
	if (AxisValue == 0.f || !GetCharacter())
		return;

	GetCharacter()->AddMovementInput(GetCharacter()->GetActorRightVector(), AxisValue);
}

void AESPlayerController::OnPitchInput(float AxisValue)
{
	if (AxisValue == 0.f)
		return;

	AddPitchInput(AxisValue * -1.f);
}

void AESPlayerController::OnYawInput(float AxisValue)
{
	if (AxisValue == 0.f)
		return;

	AddYawInput(AxisValue);
}

void AESPlayerController::ToggleCursor()
{
	SetShowMouseCursor(!ShouldShowMouseCursor());
	SetIgnoreLookInput(ShouldShowMouseCursor());

	OnCursorToggle.Broadcast();

	if (ShouldShowMouseCursor())
	{
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this);
	}
	else
	{
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
		UWidgetBlueprintLibrary::SetFocusToGameViewport();
	}
}

void AESPlayerController::OnNextItemInput()
{
	if (!GetCharacter())
		return;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetCharacter(),
	                                                         FGameplayTag::RequestGameplayTag(
		                                                         TEXT("Event.Ability.Item.Next")),
	                                                         FGameplayEventData());
}

void AESPlayerController::OnPreviousItemInput()
{
	if (!GetCharacter())
		return;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetCharacter(),
	                                                         FGameplayTag::RequestGameplayTag(
		                                                         TEXT("Event.Ability.Item.Previous")),
	                                                         FGameplayEventData());
}

void AESPlayerController::OnInvisilityInput()
{
	if (!GetCharacter())
		return;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetCharacter(),
	                                                         FGameplayTag::RequestGameplayTag(
		                                                         TEXT("Event.Ability.Invisibility")),
	                                                         FGameplayEventData());
}
