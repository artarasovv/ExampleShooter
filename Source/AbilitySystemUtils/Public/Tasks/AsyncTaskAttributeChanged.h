// Copyright, Artem Tarasov, 2021

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AbilitySystemComponent.h"
#include "AsyncTaskAttributeChanged.generated.h"

/** Attribute changed delegate declaration */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAttributeChangedSignature, FGameplayAttribute, Attribute, float,
											NewValue, float, OldValue);

/** Blueprint Async action to listen gameplay attribute changes */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class ABILITYSYSTEMUTILS_API UAsyncTaskAttributeChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/** Attribute Changed delegate */
	UPROPERTY(BlueprintAssignable)
	FAttributeChangedSignature OnAttributeChanged;

	/** Listens for an attribute changing */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTaskAttributeChanged* ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent,
																FGameplayAttribute Attribute);

	/**
	* Listens for an attribute changing
	* Version that takes in an array of Attributes. Check the Attribute output for which Attribute changed
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTaskAttributeChanged* ListenForAttributesChange(UAbilitySystemComponent* AbilitySystemComponent,
																TArray<FGameplayAttribute> Attributes);


	/**
	* You must call this function manually when you want the AsyncTask to end.
	* For UMG Widgets, you would call it in the Widget's Destruct event.
	*/
	UFUNCTION(BlueprintCallable)
	void EndTask();

	protected:
	/** Listening ASC */
	UPROPERTY()
	UAbilitySystemComponent* ASC;

	/** Listening attributes */
	FGameplayAttribute AttributeToListenFor;
	TArray<FGameplayAttribute> AttributesToListenFor;

	/** Called on any listening to attribute changed */
	void AttributeChanged(const FOnAttributeChangeData& Data) const;
};
