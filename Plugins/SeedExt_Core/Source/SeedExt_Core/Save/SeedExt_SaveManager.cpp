// Fill out your copyright notice in the Description page of Project Settings.

#include "SeedExt_SaveManager.h"

#include "SeedExt_SaveDataBase.h"
#include "SeedExt_SaveInterface.h"

#include "Kismet/GameplayStatics.h"

#include "SeedExt_Core/SeedExt_Util.h"

void USeedExt_SaveManager::RegisterSaveGame(class ISeedExt_SaveInterface* InSaveInterface)
{
	if(InSaveInterface == nullptr)
		return;

	if(SaveInterface_List.Find(InSaveInterface) == INDEX_NONE)
		SaveInterface_List.Emplace(InSaveInterface);
}
void USeedExt_SaveManager::UnregisterSaveGame(class ISeedExt_SaveInterface* InSaveInterface)
{
	if(InSaveInterface == nullptr)
		return;

	SaveInterface_List.Remove(InSaveInterface);
}

void USeedExt_SaveManager::RequestSaveAsync(class ISeedExt_SaveInterface* InSaveInterface, FSeedExt_OnSaveDataSaveResultDelegate::FDelegate InDelegate)
{
	if(InSaveInterface == nullptr)
		return;

	if(USeedExt_SaveDataBase* SlotBase = Cast<USeedExt_SaveDataBase>(UGameplayStatics::CreateSaveGameObject(InSaveInterface->GetSaveDataClass())))
	{
		InSaveInterface->OnPrepareSaveData(SlotBase);
		
		SlotBase->BindSaveResultDelegate(InSaveInterface, &InDelegate);

		UGameplayStatics::AsyncSaveGameToSlot(SlotBase, InSaveInterface->GetSlotName(), 0, FAsyncSaveGameToSlotDelegate::CreateUObject(this, &USeedExt_SaveManager::OnSaveAsyncCompleteDelegate, SlotBase));
	}
}
bool USeedExt_SaveManager::RequestSaveSync(class ISeedExt_SaveInterface* InSaveInterface) const
{
	if(InSaveInterface == nullptr)
		return false;

	if(USeedExt_SaveDataBase* SlotBase = Cast<USeedExt_SaveDataBase>(UGameplayStatics::CreateSaveGameObject(InSaveInterface->GetSaveDataClass())))
	{
		InSaveInterface->OnPrepareSaveData(SlotBase);

		return UGameplayStatics::SaveGameToSlot(SlotBase, InSaveInterface->GetSlotName(), 0);
	}

	return false;
}

void USeedExt_SaveManager::RequestLoadAsync(class ISeedExt_SaveInterface* InSaveInterface, FSeedExt_OnSaveDataLoadResultDelegate::FDelegate InDelegate)
{
	if(InSaveInterface == nullptr)
		return;

	UGameplayStatics::AsyncLoadGameFromSlot(InSaveInterface->GetSlotName(), 0, FAsyncLoadGameFromSlotDelegate::CreateUObject(this, &USeedExt_SaveManager::OnLoadAsyncCompleteDelegate, InSaveInterface, InDelegate));
}
USeedExt_SaveDataBase* USeedExt_SaveManager::RequestLoadSync(class ISeedExt_SaveInterface* InSaveInterface) const
{
	if(InSaveInterface == nullptr)
		return nullptr;

	if(USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(InSaveInterface->GetSlotName(), 0))
		return Cast<USeedExt_SaveDataBase>(SaveGame);

	USeedExt_SaveDataBase* SaveGame = Cast<USeedExt_SaveDataBase>(UGameplayStatics::CreateSaveGameObject(InSaveInterface->GetSaveDataClass()));
	SaveGame->SetDefaultValue();
	SaveGame->SetUserIdentifier(InSaveInterface->GetUserIdentifier());
	
	return SaveGame;
}


void USeedExt_SaveManager::OnSaveAsyncCompleteDelegate(const FString& InSlotName, const int32 InUserIndex, bool bInIsSuccess, USeedExt_SaveDataBase* InSaveData) const
{
	if(InSaveData != nullptr)
		InSaveData->OnSaveResultDelegator.Broadcast(bInIsSuccess);
}
void USeedExt_SaveManager::OnLoadAsyncCompleteDelegate(const FString& InSlotName, const int32 InUserIndex, USaveGame* InLoadedData, ISeedExt_SaveInterface* InSaveInterface, FSeedExt_OnSaveDataLoadResultDelegate::FDelegate InDelegate) const
{
	USeedExt_SaveDataBase* SaveData = nullptr;
	if(InLoadedData != nullptr)
		SaveData = Cast<USeedExt_SaveDataBase>(InLoadedData);

	if(SaveData == nullptr)
	{
		SaveData = Cast<USeedExt_SaveDataBase>(UGameplayStatics::CreateSaveGameObject(InSaveInterface->GetSaveDataClass()));
		if(SaveData == nullptr)
		{
			InDelegate.ExecuteIfBound(false, nullptr);
			return;
		}
		
		SaveData->SetDefaultValue();
		SaveData->SetUserIdentifier(InSaveInterface->GetUserIdentifier());
		UGameplayStatics::SaveGameToSlot(SaveData, InSaveInterface->GetSlotName(), 0);
	}

	if(InDelegate.IsBound() == true)
		SaveData->BindLoadResultDelegate(InSaveInterface, &InDelegate);
	
	SaveData->OnLoadResultDelegator.Broadcast(true, SaveData);
}
