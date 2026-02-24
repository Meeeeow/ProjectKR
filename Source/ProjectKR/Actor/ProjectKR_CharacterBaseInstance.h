// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectKR_CharacterBaseInstance.generated.h"

UCLASS()
class PROJECTKR_API AProjectKR_CharacterBaseInstance : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AProjectKR_CharacterBaseInstance();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
