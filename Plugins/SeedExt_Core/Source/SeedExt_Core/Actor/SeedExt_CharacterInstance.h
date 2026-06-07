// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SeedExt_AnimInstance.h"

#include "GameFramework/Character.h"
#include "SeedExt_CharacterInstance.generated.h"

UCLASS(Blueprintable)
class SEEDEXT_CORE_API ASeedExt_CharacterInstance : public ACharacter
{
	GENERATED_BODY()

public:
	ASeedExt_CharacterInstance();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	void SetAnimation(FName InAlias, class UAnimSequence* InAnimSequence, ESeedExt_DirectionAnimType InDirectionAnimType);
};
