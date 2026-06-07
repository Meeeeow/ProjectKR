// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "ProjectKR_AbilityInputConfig.generated.h"

USTRUCT(BlueprintType)
struct PROJECTKR_API FProjectKR_AbilityInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TObjectPtr<class UInputAction> InputAction = nullptr;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag InputTag;
};

UCLASS(ClassGroup=(ProjectKR))
class PROJECTKR_API UProjectKR_AbilityInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	const class UInputAction* GetInputActionByTag(const FGameplayTag& InInputTag) const;

protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="KR|Input")
	TArray<FProjectKR_AbilityInputAction> AbilityInputAction_List;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="KR|Input")
	TArray<FProjectKR_AbilityInputAction> NativeInputAction_List;
};
