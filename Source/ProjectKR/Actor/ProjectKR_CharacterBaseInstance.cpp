// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_CharacterBaseInstance.h"

AProjectKR_CharacterBaseInstance::AProjectKR_CharacterBaseInstance()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AProjectKR_CharacterBaseInstance::BeginPlay()
{
	Super::BeginPlay();
	
}
void AProjectKR_CharacterBaseInstance::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AProjectKR_CharacterBaseInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectKR_CharacterBaseInstance::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

