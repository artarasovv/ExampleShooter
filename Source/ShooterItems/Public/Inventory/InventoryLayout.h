// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "InventorySlot.h"
#include "Engine/DataAsset.h"
#include "InventoryLayout.generated.h"

/** Used to make presets of inventory layout */
UCLASS(BlueprintType, Blueprintable)
class SHOOTERITEMS_API UInventoryLayout : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** List of slots types */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ExampleShooter | Slot")
	TArray<EInventorySlot> InventorySlotsTypes;
};
