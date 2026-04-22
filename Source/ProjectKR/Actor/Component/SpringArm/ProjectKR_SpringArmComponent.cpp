// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_SpringArmComponent.h"

UProjectKR_SpringArmComponent::UProjectKR_SpringArmComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ProbeSize = 18.f;
}

// Called when the game starts
void UProjectKR_SpringArmComponent::BeginPlay()
{
	Super::BeginPlay();
}
void UProjectKR_SpringArmComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
void UProjectKR_SpringArmComponent::UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime)
{
	Super::UpdateDesiredArmLocation(bDoTrace, bDoLocationLag, bDoRotationLag, DeltaTime);
}

void UProjectKR_SpringArmComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

