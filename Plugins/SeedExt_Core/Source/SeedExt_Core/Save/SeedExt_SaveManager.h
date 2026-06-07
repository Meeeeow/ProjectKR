// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SeedExt_SaveDefine.h"

#include "SeedExt_Core/Delegate/SeedExt_DelegateWrapper.h"
#include "SeedExt_Core/SubSystem/SeedExt_SubSystemBase.h"
#include "SeedExt_SaveManager.generated.h"

#define SeedExt_GetSaveManager(World) FSeedExt_SubSystemCollector::_GetSubSystem<USeedExt_SaveManager>(World)
#define SeedExt_HasSaveManager(World) FSeedExt_SubSystemCollector::_HasSubSystem<USeedExt_SaveManager>(World)

UCLASS(ClassGroup=(SeedExt_Core))
class SEEDEXT_CORE_API USeedExt_SaveManager : public USeedExt_GameInstanceSubSystem
{
	GENERATED_BODY()

public:
	void RegisterSaveGame(class ISeedExt_SaveInterface* InSaveInterface);
	void UnregisterSaveGame(class ISeedExt_SaveInterface* InSaveInterface);

	void RequestSaveAsync(class ISeedExt_SaveInterface* InSaveInterface, FSeedExt_OnSaveDataSaveResultDelegate::FDelegate InDelegate);
	bool RequestSaveSync(class ISeedExt_SaveInterface* InSaveInterface) const;

	void RequestLoadAsync(class ISeedExt_SaveInterface* InSaveInterface, FSeedExt_OnSaveDataLoadResultDelegate::FDelegate InDelegate);
	class USeedExt_SaveDataBase* RequestLoadSync(class ISeedExt_SaveInterface* InSaveInterface) const;
	
private:
	TArray<TWeakInterfacePtr<class ISeedExt_SaveInterface>> SaveInterface_List;

	void OnSaveAsyncCompleteDelegate(const FString& InSlotName, const int32 InUserIndex, bool bInIsSuccess, USeedExt_SaveDataBase* InSaveData) const;
	void OnLoadAsyncCompleteDelegate(const FString& InSlotName, const int32 InUserIndex, USaveGame* InLoadedData, class ISeedExt_SaveInterface* InSaveInterface, FSeedExt_OnSaveDataLoadResultDelegate::FDelegate InDelegate) const;
};
