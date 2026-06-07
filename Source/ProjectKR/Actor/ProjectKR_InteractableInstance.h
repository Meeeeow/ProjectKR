// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SeedExt_Core/Actor/SeedExt_CharacterInstance.h"
#include "ProjectKR_InteractableInstance.generated.h"

UCLASS()
class PROJECTKR_API AProjectKR_InteractableInstance : public ASeedExt_CharacterInstance
{
	GENERATED_BODY()

public:
	AProjectKR_InteractableInstance();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
