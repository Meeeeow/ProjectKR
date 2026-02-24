// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectKR_MoveableCharacterInstance.h"
#include "ProjectKR_PlayerCharacterInstance.generated.h"

UCLASS()
class PROJECTKR_API AProjectKR_PlayerCharacterInstance : public AProjectKR_MoveableCharacterInstance
{
	GENERATED_BODY()

public:
	AProjectKR_PlayerCharacterInstance();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
