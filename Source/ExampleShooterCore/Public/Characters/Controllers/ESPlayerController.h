// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ESPlayerController.generated.h"

/** Declare delegate that will be used to hide hint animation */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCursorToggleSignature);

/** Player controller class. Receives inputs. Used as bridge between client and server */
UCLASS()
class EXAMPLESHOOTERCORE_API AESPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AESPlayerController(const FObjectInitializer& ObjectInitializer);

	/** Called when controller successfully possessed pawn. Called on client */
	virtual void AcknowledgePossession(APawn* P) override;

	/** Used to setup player input */
	virtual void SetupInputComponent() override;

	/** Controlled pawn ASC Interface */
	UPROPERTY(BlueprintReadOnly, Category = "ExampleShooter | Interfaces")
	TScriptInterface<class IAbilitySystemInterface> PawnASCInterface;

	/** Broadcasts when cursor is toggled */
	UPROPERTY(BlueprintAssignable)
	FCursorToggleSignature OnCursorToggle;

	UFUNCTION(BlueprintCallable)
	void ToggleCursor();

protected:
	virtual void BeginPlay() override;
	
	/** Sends RPC to server. Server check if player can initiate Server travel. If true, then travel */
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void TryInitiateServerTravel(const FString& InURL,
	                             bool bAbsolute,
	                             bool bShouldSkipGameNotify);

	/** Sends RPC to server. Server check if player can spawn item. If true, then spawns item */
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void TrySpawnItem(TSubclassOf<class AESItemBase> ItemClass);

	/** Server ONLY. Checks if player can initiate server travel. If Called on client will return false */
	virtual bool CanPlayerInitiateServerTravel() const;

	/** Server ONLY. Checks if player can spawn item. If Called on client will return false */
	virtual bool CanPlayerSpawnItem() const;
	
private:	
	void OnMoveForwardInput(float AxisValue);
	void OnMoveRightInput(float AxisValue);
	void OnPitchInput(float AxisValue);
	void OnYawInput(float AxisValue);

	void OnNextItemInput();
	void OnPreviousItemInput();
	void OnInvisilityInput();
};
