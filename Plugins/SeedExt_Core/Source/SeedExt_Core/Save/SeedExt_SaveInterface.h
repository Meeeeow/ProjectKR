// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SeedExt_SaveInterface.generated.h"

UINTERFACE()
class USeedExt_SaveInterface : public UInterface
{
	GENERATED_BODY()
};

class SEEDEXT_CORE_API ISeedExt_SaveInterface
{
	GENERATED_BODY()

public:
	virtual FString GetSlotName() const PURE_VIRTUAL( ISeedExt_SaveInterface::GetSlotName,return TEXT(""); )
	virtual class USeedExt_SaveDataBase* GetSaveData() = 0;
	virtual TSubclassOf<class USeedExt_SaveDataBase> GetSaveDataClass() const PURE_VIRTUAL( ISeedExt_SaveInterface::GetSaveDataClass, return nullptr; )
	uint32 GetUserIdentifier() const { return UserIdentifier; }
	void SetUserIdentifier(const uint32 InUserIdentifier) { UserIdentifier = InUserIdentifier; }
	bool HasUserIdentifier() const { return UserIdentifier != 0; }
	
	virtual void OnPrepareSaveData(class USeedExt_SaveDataBase* InSaveData) = 0;

protected:
	uint32 UserIdentifier = 0;
};
