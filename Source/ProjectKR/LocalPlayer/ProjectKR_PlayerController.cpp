// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_PlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "InputMappingContext.h"

#include "GameFramework/GameModeBase.h"

#include "ProjectKR/ProjectKR_LocalPlayer.h"
#include "ProjectKR/Ability/ProjectKR_AbilitySystemComponent.h"
#include "ProjectKR/Ability/ProjectKR_GameplayTags.h"

#include "SeedExt_Core/SubSystem/SeedExt_SubSystemCollector.h"

#include "ProjectKR/Actor/ProjectKR_PlayerCharacterInstance.h"
#include "ProjectKR/Input/ProjectKR_InputAction.h"
#include "ProjectKR/Input/ProjectKR_InputSubSystem.h"

#include "SeedExt_Core/Actor/SeedExt_ActorManager.h"

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
}
void AProjectKR_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if(EnhancedInputComponent == nullptr)
		return;

	BindInputAction(EnhancedInputComponent);
}

void AProjectKR_PlayerController::OnCharacterSpawnComplete(FSeedExt_CharacterHandle InCharacterHandle, FSoftObjectPath InCharacterPath)
{
	USeedExt_ActorManager* ActorManager = SeedExt_GetActorManager(GetWorld());
	if(ActorManager == nullptr)
		return;

	ASeedExt_CharacterInstance* CharacterInstance = ActorManager->GetCharacterInstance(InCharacterHandle);
	if(IsValid(CharacterInstance) == false)
		return;

	CharacterInstance->SetActorHiddenInGame(false);
	Possess(CharacterInstance);
}
void AProjectKR_PlayerController::OnCharacterSpawnFail(FSeedExt_CharacterHandle InCharacterHandle, FSoftObjectPath InCharacterPath)
{
	// Do Nothing...
}

void AProjectKR_PlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

AProjectKR_PlayerCharacterInstance* AProjectKR_PlayerController::GetPlayerCharacterInstance()
{
	APawn* MyPawn = GetPawn();
	if(IsValid(MyPawn) == false)
		return nullptr;

	return Cast<AProjectKR_PlayerCharacterInstance>(MyPawn);
}

void AProjectKR_PlayerController::ClientRPC_OnPlayerReadyComplete_Implementation(const FSoftObjectPath& InCharacterPath)
{
	if(InCharacterPath.IsValid() == false)
		return;

	Server_RequestSpawn(InCharacterPath);
}
void AProjectKR_PlayerController::Server_RequestSpawn_Implementation(const FSoftObjectPath& InCharacterPath)
{
	if(InCharacterPath.IsValid() == false)
		return;

	USeedExt_ActorManager* ActorManager = SeedExt_GetActorManager(GetWorld());
	if(ActorManager == nullptr)
		return;

	FSeedExt_ActorSpawnRequestInfo ActorSpawnRequestInfo;
	if(AActor* FoundStartSpot = GetWorld()->GetAuthGameMode()->FindPlayerStart(this))
	{
		ActorSpawnRequestInfo.SpawnLocation = FoundStartSpot->GetActorLocation();
		ActorSpawnRequestInfo.SpawnRotation = FoundStartSpot->GetActorRotation();
	}
	ActorSpawnRequestInfo.SpawnScale = FVector::OneVector;
	ActorSpawnRequestInfo.bIsHideSpawn = false;

	FSeedExt_CharacterSpawnCompleteDelegate::FDelegate SpawnCompleteDelegate = FSeedExt_CharacterSpawnCompleteDelegate::FDelegate::CreateUObject(this,&AProjectKR_PlayerController::OnCharacterSpawnComplete);
	FSeedExt_CharacterSpawnFailDelegate::FDelegate SpawnFailDelegate = FSeedExt_CharacterSpawnFailDelegate::FDelegate::CreateUObject(this, &AProjectKR_PlayerController::OnCharacterSpawnFail);

	ActorSpawnRequestInfo.SpawnCompleteDelegate = &SpawnCompleteDelegate;
	ActorSpawnRequestInfo.SpawnFailDelegate = &SpawnFailDelegate;

	ActorManager->SpawnCharacter(InCharacterPath, ActorSpawnRequestInfo);
}

void AProjectKR_PlayerController::BindInputAction(class UEnhancedInputComponent* InEnhancedInputComponent)
{
	if(InEnhancedInputComponent == nullptr)
		return;

	if(UProjectKR_InputSubSystem* InputManager = ProjectKR_GetInputManager(GetWorld()))
	{
		InputManager->SetPlayerController(this);
		const FProjectKR_InputSource* InputSource = InputManager->GetPlayingInputConfigSource();
		if(InputSource==nullptr || InputSource->InputMappingContext==nullptr)
			return;

		ULocalPlayer* LocalPlayer = GetLocalPlayer();
		if(LocalPlayer == nullptr)
			return;

		UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if(EnhancedInputSubsystem == nullptr)
			return;

		EnhancedInputSubsystem->AddMappingContext(InputSource->InputMappingContext, 0);

		const TArray<FEnhancedActionKeyMapping>& ActionKeyMapping_List = InputSource->InputMappingContext->GetMappings();
		TSet<TObjectPtr<const UInputAction>> BoundedAction_List;
		BoundedAction_List.Reserve(ActionKeyMapping_List.Num());

		for(int32 Index=0; Index<ActionKeyMapping_List.Num(); ++Index)
		{
			const FEnhancedActionKeyMapping& ActionKeyMapping = ActionKeyMapping_List[Index];
			if(ActionKeyMapping.Action == nullptr)
				continue;

			if(BoundedAction_List.Contains(ActionKeyMapping.Action) == true)
				continue;

			BoundedAction_List.Emplace(ActionKeyMapping.Action);

			const UProjectKR_InputAction* InputAction = Cast<UProjectKR_InputAction>(ActionKeyMapping.Action);
			if(InputAction == nullptr)
				continue;

			const FGameplayTag& InputTag = InputAction->InputTag;
			if(InputTag.IsValid() == false)
				continue;

			if(InputTag.MatchesTagExact(ProjectKR_Tags::InputTag_Move) == true)
			{
				InEnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, &AProjectKR_PlayerController::Input_Move);
			}
			else if(InputTag.MatchesTagExact(ProjectKR_Tags::InputTag_Look) == true)
			{
				InEnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, &AProjectKR_PlayerController::Input_Look);
			}
			else if(InputTag.MatchesTagExact(ProjectKR_Tags::InputTag_Jump) == true)
			{
				InEnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, &AProjectKR_PlayerController::Input_JumpStart);
				InEnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Canceled, this, &AProjectKR_PlayerController::Input_JumpComplete);
			}
			else
			{
				InEnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, &AProjectKR_PlayerController::Input_AbilityInputTagPressed, InputTag);
				InEnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Completed, this, &AProjectKR_PlayerController::Input_AbilityInputTagReleased, InputTag);
			}
		}
	}
}
void AProjectKR_PlayerController::UnbindInputAction()
{
	
}

void AProjectKR_PlayerController::Input_Move(const FInputActionValue& InInputActionValue)
{
	AProjectKR_PlayerCharacterInstance* PlayerCharacterInstance = GetPlayerCharacterInstance();
	if(PlayerCharacterInstance == nullptr)
		return;

	PlayerCharacterInstance->HandleMoveInput(InInputActionValue.Get<FVector2D>());
}
void AProjectKR_PlayerController::Input_Look(const FInputActionValue& InInputActionValue)
{
	AProjectKR_PlayerCharacterInstance* PlayerCharacterInstance = GetPlayerCharacterInstance();
	if(PlayerCharacterInstance == nullptr)
		return;

	PlayerCharacterInstance->HandleLookInput(InInputActionValue.Get<FVector2D>());
}
void AProjectKR_PlayerController::Input_JumpStart(const FInputActionValue& InInputActionValue)
{
	AProjectKR_PlayerCharacterInstance* PlayerCharacterInstance = GetPlayerCharacterInstance();
	if(PlayerCharacterInstance == nullptr)
		return;

	PlayerCharacterInstance->HandleJumpStart();
}
void AProjectKR_PlayerController::Input_JumpComplete(const FInputActionValue& InInputActionValue)
{
	AProjectKR_PlayerCharacterInstance* PlayerCharacterInstance = GetPlayerCharacterInstance();
	if(PlayerCharacterInstance == nullptr)
		return;

	PlayerCharacterInstance->HandleJumpStop();
}

void AProjectKR_PlayerController::Input_AbilityInputTagPressed(FGameplayTag InGameplayTag)
{
	AProjectKR_PlayerCharacterInstance* PlayerCharacterInstance = GetPlayerCharacterInstance();
	if(PlayerCharacterInstance == nullptr)
		return;

	if(UProjectKR_AbilitySystemComponent* AbilitySystemComponent = Cast<UProjectKR_AbilitySystemComponent>(PlayerCharacterInstance->GetAbilitySystemComponent()))
		AbilitySystemComponent->InputTagPressed(InGameplayTag);
}
void AProjectKR_PlayerController::Input_AbilityInputTagReleased(FGameplayTag InGameplayTag)
{
	AProjectKR_PlayerCharacterInstance* PlayerCharacterInstance = GetPlayerCharacterInstance();
	if(PlayerCharacterInstance == nullptr)
		return;

	if(UProjectKR_AbilitySystemComponent* AbilitySystemComponent = Cast<UProjectKR_AbilitySystemComponent>(PlayerCharacterInstance->GetAbilitySystemComponent()))
		AbilitySystemComponent->InputTagReleased(InGameplayTag);
}

