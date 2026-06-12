// Fill out your copyright notice in the Description page of Project Settings.

#include "SeedExt_AnimInstance.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "SeedExt_Core/SeedExt_Util.h"

void FSeedExt_DirectionAnimLocomotionInfo::SetAnimation(UAnimSequence* InAnimation, ESeedExt_DirectionAnimType InDirectionAnimType)
{
	Animation_List.Add(InDirectionAnimType, InAnimation);
}
TObjectPtr<UAnimSequence> FSeedExt_DirectionAnimLocomotionInfo::GetAnimation(ESeedExt_DirectionAnimType InDirectionAnimType)
{
	return SeedExt_GetSafeMapValue(Animation_List.Find(InDirectionAnimType));
}

void USeedExt_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}
void USeedExt_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* OwningPawn = TryGetPawnOwner();
	if(OwningPawn == nullptr)
		return;

	const FVector Velocity = OwningPawn->GetVelocity();
	const FVector GroundVelocity(Velocity.X, Velocity.Y, 0);

	Speed = GroundVelocity.Size();
	bIsMove = (Speed > 3.f);
	Direction = UKismetAnimationLibrary::CalculateDirection(GroundVelocity, OwningPawn->GetActorRotation());

	if(const ACharacter* OwningCharacter = Cast<ACharacter>(OwningPawn))
	{
		if(const UCharacterMovementComponent* MovementComponent = OwningCharacter->GetCharacterMovement())
			bIsFall = MovementComponent->IsFalling();
	}
}
