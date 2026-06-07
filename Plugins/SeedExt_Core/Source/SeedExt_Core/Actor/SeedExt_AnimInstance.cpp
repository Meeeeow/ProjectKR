// Fill out your copyright notice in the Description page of Project Settings.

#include "SeedExt_AnimInstance.h"

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
}
