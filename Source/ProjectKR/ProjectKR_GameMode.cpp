// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_GameMode.h"

#include "ProjectKR_PlayerState.h"

#include "LocalPlayer/ProjectKR_PlayerController.h"

#include "SeedExt_Core/Actor/SeedExt_ActorManager.h"
#include "SeedExt_Core/Save/SeedExt_SaveManager.h"
#include "SeedExt_Core/SubSystem/SeedExt_SubSystemCollector.h"

void AProjectKR_GameMode::BeginPlay()
{
	Super::BeginPlay();
}
void AProjectKR_GameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AProjectKR_GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

}

void AProjectKR_GameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if(NewPlayer == nullptr)
		return;

	AProjectKR_PlayerState* PlayerState = NewPlayer->GetPlayerState<AProjectKR_PlayerState>();
	if(PlayerState == nullptr)
		return;

	if(PlayerState->HasUserIdentifier() == false)
	{
		uint32 UserIdentifier = 0;
		const FUniqueNetIdRepl& UniqueID = PlayerState->GetUniqueId();
		if(UniqueID.IsValid()==true && UniqueID.GetUniqueNetId().IsValid()==true)
			UserIdentifier = GetTypeHash(*UniqueID.GetUniqueNetId());

		// Standalone
		if(UserIdentifier == 0)
		{
			static std::atomic<uint32> LocalPlayerCounter{0};
			UserIdentifier = ++LocalPlayerCounter;
		}

		PlayerState->SetUserIdentifier(UserIdentifier);
	}

	USeedExt_SaveManager* SaveManager = SeedExt_GetSaveManager(GetWorld());
	if(SaveManager == nullptr)
		return;

	SaveManager->RegisterSaveGame(PlayerState);

	FSeedExt_OnSaveDataLoadResultDelegate::FDelegate LoadDelegate = FSeedExt_OnSaveDataLoadResultDelegate::FDelegate::CreateWeakLambda(NewPlayer, [this, NewPlayer](const bool bInIsSuccess, const USeedExt_SaveDataBase* InSaveData)
	{
		this->OnPlayerDataLoaded(NewPlayer, bInIsSuccess, InSaveData);
	});

	SaveManager->RequestLoadAsync(PlayerState, LoadDelegate);
}

void AProjectKR_GameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
}

void AProjectKR_GameMode::OnPlayerDataLoaded(APlayerController* InPlayerController, const bool bIsSuccess, const class USeedExt_SaveDataBase* InSaveData)
{
	if(InPlayerController == nullptr)
		return;

	AProjectKR_PlayerController* PlayerController = Cast<AProjectKR_PlayerController>(InPlayerController);
	if(PlayerController == nullptr)
		return;
	
	AProjectKR_PlayerState* PlayerState = PlayerController->GetPlayerState<AProjectKR_PlayerState>();
	if(PlayerState == nullptr)
		return;

	if(bIsSuccess == false)
		return;

	const UProjectKR_PlayerStateSaveData* PlayerStateSaveData = Cast<UProjectKR_PlayerStateSaveData>(InSaveData);
	if(PlayerStateSaveData == nullptr)
		return;

	const FSoftObjectPath& CharacterPath = PlayerStateSaveData->GetCharacterSoftObjectPath();
	PlayerController->ClientRPC_OnPlayerReadyComplete(CharacterPath);
}