// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectKR/Actor/ProjectKR_CharacterBaseInstance.h"
#include "ProjectKR_MoveableCharacterInstance.generated.h"

UCLASS()
class PROJECTKR_API AProjectKR_MoveableCharacterInstance : public AProjectKR_CharacterBaseInstance
{
	GENERATED_BODY()

public:
	AProjectKR_MoveableCharacterInstance();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
