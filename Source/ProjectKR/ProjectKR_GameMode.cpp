// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_GameMode.h"

#include "SeedExt_Core/Actor/SeedExt_ActorManager.h"

void AProjectKR_GameMode::BeginPlay()
{
	Super::BeginPlay();
}
void AProjectKR_GameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AProjectKR_GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

}

void AProjectKR_GameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

void AProjectKR_GameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
}
