// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_PlayerCharacterInstance.h"

#include "EnhancedInputComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Component/SpringArm/ProjectKR_SpringArmComponent.h"

#include "ProjectKR/Ability/ProjectKR_AbilityInputConfig.h"
#include "ProjectKR/Ability/ProjectKR_AbilitySystemComponent.h"

AProjectKR_PlayerCharacterInstance::AProjectKR_PlayerCharacterInstance()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	
	Super::SetReplicateMovement(true);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	if(UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->bOrientRotationToMovement = true;
		MovementComponent->RotationRate = FRotator(0.f, 540.f, 0.f);
		MovementComponent->JumpZVelocity = 600.f;
		MovementComponent->AirControl = 0.35f;
		MovementComponent->MinAnalogWalkSpeed = 20.f;
		MovementComponent->BrakingDecelerationWalking = 2000.f;
	}
	
	{
		static FName SpringArmComponentName(TEXT("SpringArmComponent"));
		if(SpringArmComponent == nullptr)
			SpringArmComponent = CreateDefaultSubobject<UProjectKR_SpringArmComponent>(SpringArmComponentName);

		SpringArmComponent->SetupAttachment(RootComponent);
		SpringArmComponent->TargetArmLength = 400.f;
		SpringArmComponent->bUsePawnControlRotation = true;
		SpringArmComponent->bEnableCameraLag = true;
		SpringArmComponent->CameraLagSpeed = 12.f;
		SpringArmComponent->bDoCollisionTest = true;
	}
	
	{
		static FName CameraComponentName(TEXT("KR_CameraComponent"));
		if(CameraComponent == nullptr)
			CameraComponent = CreateDefaultSubobject<UCameraComponent>(CameraComponentName);

		CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
		CameraComponent->bUsePawnControlRotation = true;
	}

	{
		static FName AbilitySystemComponentName(TEXT("KR_AbilitySystemComponent"));
		if(AbilitySystemComponent == nullptr)
			AbilitySystemComponent = CreateDefaultSubobject<UProjectKR_AbilitySystemComponent>(AbilitySystemComponentName);

		AbilitySystemComponent->SetIsReplicated(true);
		AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	}
}

void AProjectKR_PlayerCharacterInstance::BeginPlay()
{
	Super::BeginPlay();
}
void AProjectKR_PlayerCharacterInstance::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AProjectKR_PlayerCharacterInstance::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}
void AProjectKR_PlayerCharacterInstance::UnPossessed()
{
	Super::UnPossessed();
}

void AProjectKR_PlayerCharacterInstance::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void AProjectKR_PlayerCharacterInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AProjectKR_PlayerCharacterInstance::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if(EnhancedInputComponent == nullptr)
		return;
}
UAbilitySystemComponent* AProjectKR_PlayerCharacterInstance::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AProjectKR_PlayerCharacterInstance::HandleMoveInput(const FVector2D& InAxis)
{
	const FRotator YawRotation(0.f, GetController()->GetControlRotation().Yaw, 0.0f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, InAxis.Y);
	AddMovementInput(RightDirection, InAxis.X);
}
void AProjectKR_PlayerCharacterInstance::HandleLookInput(const FVector2D& InAxis)
{
	AddControllerYawInput(InAxis.X);
	AddControllerPitchInput(InAxis.Y);
}
void AProjectKR_PlayerCharacterInstance::HandleJumpStart()
{
	
}
void AProjectKR_PlayerCharacterInstance::HandleJumpStop()
{
	
}

