// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemAttachInterface.generated.h"

UINTERFACE(BlueprintType)
class UItemAttachInterface : public UInterface
{
	GENERATED_BODY()
};

/** Interface used to get relevant data without hard dependencies */
class SHOOTERITEMS_API IItemAttachInterface
{
	GENERATED_BODY()

public:
	/** Returns character arms or body mesh depending on NET local role */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	class USkeletalMeshComponent* GetNetRelevantMesh() const;
};
