// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "ProjectKR_InputAction.generated.h"

UCLASS()
class PROJECTKR_API UProjectKR_InputAction : public UInputAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly,Category="KR|Input")
	FGameplayTag InputTag;
};
