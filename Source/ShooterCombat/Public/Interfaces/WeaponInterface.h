// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"

UINTERFACE(BlueprintType)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

/** Character that can own weapons should implement this on order to minimize hard class dependencies */
class SHOOTERCOMBAT_API IWeaponInterface
{
	GENERATED_BODY()

public:
	/** Note: Can return false if no current active weapon */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetWeaponMuzzleTransform(FTransform& OutTransform) const;
};
