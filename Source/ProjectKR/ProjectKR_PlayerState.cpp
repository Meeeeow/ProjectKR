// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_PlayerState.h"

#include "SeedExt_Core/Save/SeedExt_SaveManager.h"
#include "SeedExt_Core/SubSystem/SeedExt_SubSystemCollector.h"

FString AProjectKR_PlayerState::GetSlotName() const
{
	return FString::Format(*UserIdentifierIndex, {UserIdentifier});
}

USeedExt_SaveDataBase* AProjectKR_PlayerState::GetSaveData()
{
	USeedExt_SaveManager* SaveManager = SeedExt_GetSaveManager(GetWorld());
	if(SaveManager == nullptr)
		return nullptr;

	return SaveManager->RequestLoadSync(this);
}

TSubclassOf<USeedExt_SaveDataBase> AProjectKR_PlayerState::GetSaveDataClass() const
{
	return UProjectKR_PlayerStateSaveData::StaticClass();
}

void AProjectKR_PlayerState::OnPrepareSaveData(class USeedExt_SaveDataBase* InSaveData)
{
	
}
