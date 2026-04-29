// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SeedExt_Core/SubSystem/SeedExt_SubSystemBase.h"
#include "SeedExt_Core/Handle/SeedExt_HandleDeclare.h"

#include "SeedExt_ActorManager.generated.h"

#define SeedExt_GetActorManager(World) FSeedExt_SubSystemCollector::_GetSubSystem<UProjectKR_ActorManager>(World)
#define SeedExt_HasActorManager(World) FSeedExt_SubSystemCollector::_HasSubSystem<UProjectKR_ActorManager>(World)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DECLARE_MULTICAST_DELEGATE_TwoParams(FSeedExt_CharacterSpawnCompleteDelegate, FSeedExt_CharacterHandle, FSoftObjectPath);
DECLARE_MULTICAST_DELEGATE_TwoParams(FSeedExt_CharacterSpawnFailDelegate, FSeedExt_CharacterHandle, FSoftObjectPath);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SEEDEXT_CORE_API FSeedExt_ActorSpawnRequestInfo
{
	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	FVector SpawnScale = FVector::ZeroVector;
	bool bIsHideSpawn = false;
	
	FSeedExt_CharacterSpawnCompleteDelegate::FDelegate* SpawnCompleteDelegate = nullptr;
	FSeedExt_CharacterSpawnFailDelegate::FDelegate* SpawnFailDelegate = nullptr;
};

struct SEEDEXT_CORE_API FSeedExt_ActorSpawnStatus
{
	friend class USeedExt_ActorManager;
	friend class ASeedExt_CharacterInstance;

private:
	FSeedExt_CharacterHandle CharacterHandle = FSeedExt_CharacterHandle();
	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	FVector SpawnScale = FVector::ZeroVector;
	bool bIsHideSpawn = false;
	
	FSeedExt_CharacterSpawnCompleteDelegate::FDelegate SpawnCompleteDelegate;
	FSeedExt_CharacterSpawnFailDelegate::FDelegate SpawnFailDelegate;

public:
	FSeedExt_ActorSpawnStatus() =delete;
	FSeedExt_ActorSpawnStatus(const FSeedExt_ActorSpawnRequestInfo& InRequestInfo, const FSeedExt_CharacterHandle& InCharacterHandle);
};


UCLASS()
class SEEDEXT_CORE_API USeedExt_ActorManager : public USeedExt_GameInstanceSubSystem
{
	GENERATED_BODY()

public:
	FSeedExt_CharacterHandle SpawnCharacter(const FSoftObjectPath& InSoftObjectPath, const FSeedExt_ActorSpawnRequestInfo& InRequestInfo);
	void DespawnCharacter(const FSeedExt_CharacterHandle& InCharacterHandle);

private:
	TMap<FSeedExt_CharacterHandle, FSeedExt_ActorSpawnStatus> CharacterSpawnStatus_List;
	TMap<FSeedExt_CharacterHandle, TObjectPtr<class ASeedExt_CharacterInstance>> CharacterInstance_List;

	virtual int32 GetSubSystemOrder() override { return static_cast<int32>(ESeedExt_SubSystemOrderType::Normal)-1; }
};
