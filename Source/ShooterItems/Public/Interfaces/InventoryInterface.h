// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryInterface.generated.h"

UINTERFACE(BlueprintType)
class UInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/** Inventory getter interface */
class SHOOTERITEMS_API IInventoryInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	class UESInventoryComponent* GetInventoryComponent() const;
};
