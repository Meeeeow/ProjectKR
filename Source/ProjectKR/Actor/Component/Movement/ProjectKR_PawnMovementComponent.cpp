// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_PawnMovementComponent.h"

// Sets default values for this component's properties
UProjectKR_PawnMovementComponent::UProjectKR_PawnMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

// Called when the game starts
void UProjectKR_PawnMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}
void UProjectKR_PawnMovementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void UProjectKR_PawnMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(PawnOwner==nullptr || UpdatedComponent==nullptr || ShouldSkipUpdate(DeltaTime)==true)
		return;

	FVector DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.0f) * DeltaTime * 10.0f;
	if(DesiredMovementThisFrame.IsNearlyZero() == false){
		FHitResult Hit;
		SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);
		if(Hit.IsValidBlockingHit() == true)
			SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
	}
}

