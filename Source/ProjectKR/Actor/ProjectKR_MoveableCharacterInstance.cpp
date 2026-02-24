// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_MoveableCharacterInstance.h"

AProjectKR_MoveableCharacterInstance::AProjectKR_MoveableCharacterInstance()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AProjectKR_MoveableCharacterInstance::BeginPlay()
{
	Super::BeginPlay();
	
}
void AProjectKR_MoveableCharacterInstance::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AProjectKR_MoveableCharacterInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AProjectKR_MoveableCharacterInstance::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

