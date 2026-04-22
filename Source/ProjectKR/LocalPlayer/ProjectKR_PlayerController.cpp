// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_PlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "SeedExt_Core/SubSystem/SeedExt_SubSystemCollector.h"

#include "ProjectKR/Actor/ProjectKR_PlayerCharacterInstance.h"
#include "ProjectKR/Input/ProjectKR_InputSubSystem.h"


void AProjectKR_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}
void AProjectKR_PlayerController::OnUnPossess()
{
	Super::OnUnPossess();
}

void AProjectKR_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	BindInputEvent();
}
void AProjectKR_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void AProjectKR_PlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindInputEvent();
	
	Super::EndPlay(EndPlayReason);
}

AProjectKR_PlayerCharacterInstance* AProjectKR_PlayerController::GetPlayerCharacterInstance()
{
	APawn* MyPawn = GetPawn();
	if(IsValid(MyPawn) == false)
	{
		return nullptr;
	}
	
	return Cast<AProjectKR_PlayerCharacterInstance>(MyPawn);
}

void AProjectKR_PlayerController::BindInputEvent()
{
	if(UProjectKR_InputSubSystem* InputSubSystem = ProjectKR_GetInputManager(GetWorld()))
	{
		InputSubSystem->SetPlayerController(this);
		const FProjectKR_InputSource* InputSource = InputSubSystem->GetPlayingInputConfigSource();
		if(UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			EnhancedInputSubSystem->AddMappingContext(InputSource->InputMappingContext, 0);
			if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
			{
				for(TMap<FName, UInputAction*>::TConstIterator It(InputSource->InputAction_List); It; ++It)
				{
					FName FunctionName = FName(FString::Printf(TEXT("OnHandleInput%s"),*It->Key.ToString()));
					EnhancedInputComponent->BindAction(It->Value, ETriggerEvent::Triggered, this, FunctionName);
					EnhancedInputComponent->BindAction(It->Value, ETriggerEvent::Started, this, FunctionName);
					EnhancedInputComponent->BindAction(It->Value, ETriggerEvent::Ongoing, this, FunctionName);
					EnhancedInputComponent->BindAction(It->Value, ETriggerEvent::Canceled, this, FunctionName);
					EnhancedInputComponent->BindAction(It->Value, ETriggerEvent::Completed, this, FunctionName);
				}
			}
		}
	}
}
void AProjectKR_PlayerController::UnbindInputEvent()
{
}

void AProjectKR_PlayerController::OnHandleInputMove(const struct FInputActionValue& InInputActionValue)
{
	
}
void AProjectKR_PlayerController::OnHandleInputLook(const struct FInputActionValue& InInputActionValue)
{
	
}
void AProjectKR_PlayerController::OnHandleInputJump(const struct FInputActionValue& InInputActionValue)
{
	
}
