// Fill out your copyright notice in the Description page of Project Settings.

#include "SeedExt_ActorManager.h"

FSeedExt_ActorSpawnStatus::FSeedExt_ActorSpawnStatus(const FSeedExt_ActorSpawnRequestInfo& InRequestInfo, const FSeedExt_CharacterHandle& InCharacterHandle)
{
	CharacterHandle = InCharacterHandle;
	SpawnLocation = InRequestInfo.SpawnLocation;
	SpawnRotation = InRequestInfo.SpawnRotation;
	SpawnScale = InRequestInfo.SpawnScale;
	bIsHideSpawn = InRequestInfo.bIsHideSpawn;

	SpawnCompleteDelegate = *InRequestInfo.SpawnCompleteDelegate;
	SpawnFailDelegate = *InRequestInfo.SpawnFailDelegate;
}

FSeedExt_CharacterHandle USeedExt_ActorManager::SpawnCharacter(const FSoftObjectPath& InSoftObjectPath, const FSeedExt_ActorSpawnRequestInfo& InRequestInfo)
{
	FSeedExt_CharacterHandle Handle = FSeedExt_CharacterHandle::_CreateHandle(nullptr);

	FSeedExt_ActorSpawnStatus ActorSpawnStatus(InRequestInfo, Handle);
	
	
	return Handle;
}

void USeedExt_ActorManager::DespawnCharacter(const FSeedExt_CharacterHandle& InCharacterHandle)
{
}
