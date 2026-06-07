// Fill out your copyright notice in the Description page of Project Settings.

#include "SeedExt_CharacterInstance.h"

ASeedExt_CharacterInstance::ASeedExt_CharacterInstance()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASeedExt_CharacterInstance::BeginPlay()
{
	Super::BeginPlay();
}

void ASeedExt_CharacterInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void ASeedExt_CharacterInstance::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASeedExt_CharacterInstance::SetAnimation(FName InAlias, class UAnimSequence* InAnimSequence, ESeedExt_DirectionAnimType InDirectionAnimType)
{
	USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
	if(SkeletalMeshComponent == nullptr)
		return;

	UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
	if(AnimInstance == nullptr)
		return;

	if(USeedExt_AnimInstance* AnimInstance_Inner = Cast<USeedExt_AnimInstance>(AnimInstance))
	{
		if(InAlias == FName(TEXT("InputTag_Move")) == true)
			AnimInstance_Inner->Anim_Walk.SetAnimation(InAnimSequence, InDirectionAnimType);
		else if(InAlias == FName(TEXT("InputTag_Run")) == true)
			AnimInstance_Inner->Anim_Run.SetAnimation(InAnimSequence, InDirectionAnimType);
		else if(InAlias == FName(TEXT("InputTag_Idle")) == true)
			AnimInstance_Inner->Anim_Idle = InAnimSequence;
	}
}