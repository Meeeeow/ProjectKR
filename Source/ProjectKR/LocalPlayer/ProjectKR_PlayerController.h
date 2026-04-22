// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ProjectKR_PlayerController.generated.h"

UCLASS(ClassGroup=(ProjectKR))
class PROJECTKR_API AProjectKR_PlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	class AProjectKR_PlayerCharacterInstance* GetPlayerCharacterInstance();

protected:
	void BindInputEvent();
	void UnbindInputEvent();
	
	UFUNCTION() void OnHandleInputMove(const struct FInputActionValue& InInputActionValue);
	UFUNCTION() void OnHandleInputLook(const struct FInputActionValue& InInputActionValue);
	UFUNCTION() void OnHandleInputJump(const struct FInputActionValue& InInputActionValue);
};
