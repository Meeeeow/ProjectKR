// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectKR_PlayerDefine.h"

#include "GameFramework/PlayerState.h"

#include "SeedExt_Core/Save/SeedExt_SaveInterface.h"

#include "ProjectKR_PlayerState.generated.h"

UCLASS()
class PROJECTKR_API AProjectKR_PlayerState : public APlayerState, public ISeedExt_SaveInterface
{
	GENERATED_BODY()

public:
	void SetUserIdentifier(uint64 InUserIdentifier) { UserIdentifier = InUserIdentifier; }
	virtual FString GetSlotName() const override;
	virtual USeedExt_SaveDataBase* GetSaveData() override;
	virtual TSubclassOf<class USeedExt_SaveDataBase> GetSaveDataClass() const override;
	virtual void OnPrepareSaveData(class USeedExt_SaveDataBase* InSaveData) override;

private:
	uint64 UserIdentifier = 0;
	
	SEEDEXT_DECLARE_DELEGATE_WRAPPER(FProjectKR_OnPlayerLoadCompleteDelegator,FProjectKR_OnPlayerLoadComplete,PlayerLoadComplete)
	SEEDEXT_DECLARE_DELEGATE_WRAPPER(FProjectKR_OnPlayerLoadFailDelegator,FProjectKR_OnPlayerLoadFail,PlayerLoadFail)
};
