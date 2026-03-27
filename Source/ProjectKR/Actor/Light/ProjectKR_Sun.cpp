// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_Sun.h"

// Sets default values
AProjectKR_Sun::AProjectKR_Sun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AProjectKR_Sun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectKR_Sun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

