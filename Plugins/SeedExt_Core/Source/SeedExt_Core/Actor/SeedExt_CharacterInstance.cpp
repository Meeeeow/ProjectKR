// Fill out your copyright notice in the Description page of Project Settings.

#include "SeedExt_CharacterInstance.h"

// Sets default values
ASeedExt_CharacterInstance::ASeedExt_CharacterInstance()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASeedExt_CharacterInstance::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASeedExt_CharacterInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASeedExt_CharacterInstance::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

