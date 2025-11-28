// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SeedExt_Core/SeedExt_Define.h"
#include "SeedExt_Core/SubSystem/SeedExt_SubSystemDefine.h"

#include "UObject/Interface.h"

#include "SeedExt_SubSystemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USeedExt_SubSystemInterface : public UInterface
{
	GENERATED_BODY()
};

class SEEDEXT_CORE_API ISeedExt_SubSystemInterface
{
	GENERATED_BODY()

	//--------------------------------------------------//
protected:
	ESeedExt_LoadingStateType SubSystemLoadingState = ESeedExt_LoadingStateType::NotLoading;
	ESeedExt_PlayingStateType SubSystemPlayingState = ESeedExt_PlayingStateType::NotReady;

	virtual void Init();
	virtual void Reset();
	virtual void ClearAll(ESeedExt_ClearReason InClearReason);

	virtual bool IsInitialized();

	virtual void Start();
	virtual void Pause();
	virtual void Resume();
	virtual void Stop();

	virtual void Load();
	virtual void Unload();

	virtual bool Tick_System(float InDeltaSeconds) { return true; }
	virtual bool Tick_World(float InDeltaSeconds) { return true; }

	//--------------------------------------------------//
public:
	virtual bool IsLoaded();
	virtual bool IsUnLoaded();

	virtual UWorld* GetSubSystemWorld() = 0;
	virtual UClass* GetSubSystemClass() = 0;
	virtual FString GetSubSystemName() = 0;

	virtual int32 GetSubSystemOrder() { return 0; }
	virtual int32 GetSubSystemTickOrder() { return 0; }
	virtual ESeedExt_SubSystemTickType GetSubSystemTickType() const { return ESeedExt_SubSystemTickType::None; }
	virtual ESeedExt_SubSystemType GetSubSystemType() const { return ESeedExt_SubSystemType::None; }
};
