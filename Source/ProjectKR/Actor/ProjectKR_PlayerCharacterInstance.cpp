// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_PlayerCharacterInstance.h"

AProjectKR_PlayerCharacterInstance::AProjectKR_PlayerCharacterInstance()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void AProjectKR_PlayerCharacterInstance::BeginPlay()
{
	Super::BeginPlay();
	
}
void AProjectKR_PlayerCharacterInstance::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AProjectKR_PlayerCharacterInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AProjectKR_PlayerCharacterInstance::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

