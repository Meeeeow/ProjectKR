// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SeedExt_Core/Delegate/SeedExt_DelegateWrapper.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FSeedExt_OnSaveDataSaveResultDelegate, const bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FSeedExt_OnSaveDataLoadResultDelegate, const bool, const class USeedExt_SaveDataBase*);

class FSeedExt_OnSaveDataSaveResultDelegator : public FSeedExt_DelegateWrapper<FSeedExt_OnSaveDataSaveResultDelegate>
{
public:
	void Broadcast(const bool bIsSaveResult) const { OnDelegate.Broadcast(bIsSaveResult); }
};
class FSeedExt_OnSaveDataLoadResultDelegator : public FSeedExt_DelegateWrapper<FSeedExt_OnSaveDataLoadResultDelegate>
{
public:
	void Broadcast(const bool bIsLoadResult, const class USeedExt_SaveDataBase* InSaveData) const { OnDelegate.Broadcast(bIsLoadResult, InSaveData); }
};
