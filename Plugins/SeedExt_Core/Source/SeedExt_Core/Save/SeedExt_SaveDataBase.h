// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "SeedExt_Core/Delegate/SeedExt_DelegateWrapper.h"
#include "SeedExt_SaveDefine.h"

#include "SeedExt_SaveDataBase.generated.h"

UCLASS(ClassGroup="SeedExt_Core")
class SEEDEXT_CORE_API USeedExt_SaveDataBase : public USaveGame
{
	GENERATED_BODY()

	friend class USeedExt_SaveManager;

public:
	virtual FString GetSlotName() const PURE_VIRTUAL(USeedExt_SaveDataBase::GetSlotName,return TEXT("");)
	virtual void SetDefaultValue() PURE_VIRTUAL( USeedExt_SaveDataBase::SetDefaultValue() );

	uint32 GetUserIdentifier() const { return UserIdentifier;}
	bool HasUserIdentifier() const { return UserIdentifier != 0; }

protected:
	UPROPERTY(SaveGame)
	uint32 UserIdentifier = 0;
	
	void SetUserIdentifier(const uint32 InUserIdentifier) { UserIdentifier = InUserIdentifier; } 
	
	SEEDEXT_DECLARE_DELEGATE_WRAPPER(FSeedExt_OnSaveDataSaveResultDelegator,FSeedExt_OnSaveDataSaveResultDelegate,SaveResult)
	SEEDEXT_DECLARE_DELEGATE_WRAPPER(FSeedExt_OnSaveDataLoadResultDelegator,FSeedExt_OnSaveDataLoadResultDelegate,LoadResult)
};
