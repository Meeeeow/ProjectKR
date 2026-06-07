// Fill out your copyright notice in the Description page of Project Settings.

#include "SeedExt_ActorManager.h"

#include "SeedExt_CharacterInstance.h"

#include "Kismet/GameplayStatics.h"

#include "SeedExt_Core/SeedExt_Util.h"
#include "SeedExt_Core/Resource/SeedExt_ResourceManager.h"
#include "SeedExt_Core/SubSystem/SeedExt_SubSystemCollector.h"

FSeedExt_ActorSpawnStatus::FSeedExt_ActorSpawnStatus(const FSeedExt_ActorSpawnRequestInfo& InRequestInfo, const FSeedExt_CharacterHandle& InCharacterHandle)
{
	CharacterHandle = InCharacterHandle;
	SpawnLocation = InRequestInfo.SpawnLocation;
	SpawnRotation = InRequestInfo.SpawnRotation;
	SpawnScale = InRequestInfo.SpawnScale;
	bIsHideSpawn = InRequestInfo.bIsHideSpawn;

	if(InRequestInfo.SpawnCompleteDelegate != nullptr)
		SpawnCompleteDelegate = *InRequestInfo.SpawnCompleteDelegate;
	if(InRequestInfo.SpawnFailDelegate != nullptr)
		SpawnFailDelegate = *InRequestInfo.SpawnFailDelegate;
}

FSeedExt_CharacterHandle USeedExt_ActorManager::SpawnCharacter(const FSoftObjectPath& InSoftObjectPath, const FSeedExt_ActorSpawnRequestInfo& InRequestInfo)
{
	FSeedExt_CharacterHandle Handle = FSeedExt_CharacterHandle::_CreateHandle(nullptr);

	USeedExt_ResourceManager* ResourceManager = SeedExt_GetResourceManager(GetWorld());
	if(ResourceManager == nullptr)
	{
		if(InRequestInfo.SpawnFailDelegate != nullptr)
			InRequestInfo.SpawnFailDelegate->ExecuteIfBound(Handle, InSoftObjectPath);
		
		return Handle;
	}
	
	FSeedExt_ActorSpawnStatus ActorSpawnStatus(InRequestInfo, Handle);
	CharacterSpawnStatus_List.Emplace(Handle, ActorSpawnStatus);
	FSeedExt_ResourceLoadCompleteDelegate::FDelegate LoadCompleteDelegate = FSeedExt_ResourceLoadCompleteDelegate::FDelegate::CreateUObject(this, &USeedExt_ActorManager::OnLoadCharacterComplete, Handle);
	FSeedExt_ResourceLoadFailDelegate::FDelegate LoadFailDelegate = FSeedExt_ResourceLoadFailDelegate::FDelegate::CreateUObject(this, &USeedExt_ActorManager::OnLoadCharacterFail, Handle);
	SeedExt_GetResourceManager(GetWorld())->LoadObject<ASeedExt_CharacterInstance>(InSoftObjectPath, ESeedExt_ResourceType::SkeletalMesh, ESeedExt_ResourceLoadOrderType::Normal, &LoadCompleteDelegate, &LoadFailDelegate);
	
	return Handle;
}

void USeedExt_ActorManager::DespawnCharacter(const FSeedExt_CharacterHandle& InCharacterHandle)
{
	
}

void USeedExt_ActorManager::OnLoadCharacterComplete(FSoftObjectPath InSoftObjectPath, UObject* InObject, UClass* InClass, FSeedExt_CharacterHandle InCharacterHandle)
{
	FSeedExt_ActorSpawnStatus* ActorSpawnStatus = CharacterSpawnStatus_List.Find(InCharacterHandle);
	if(ActorSpawnStatus == nullptr)
		return;

	UClass* CharacterClass = Cast<UClass>(InObject);
	if(CharacterClass == nullptr)
		CharacterClass = InClass;

	ASeedExt_CharacterInstance* SpawnedCharacter = SpawnCharacterActorInternal(CharacterClass, *ActorSpawnStatus);
	if(SpawnedCharacter == nullptr)
	{
		ActorSpawnStatus->SpawnFailDelegate.ExecuteIfBound(InCharacterHandle, InSoftObjectPath);
		return;
	}
	
	CharacterInstance_List.Emplace(InCharacterHandle, SpawnedCharacter);
	ActorSpawnStatus->SpawnCompleteDelegate.ExecuteIfBound(InCharacterHandle, InSoftObjectPath);

	CharacterSpawnStatus_List.Remove(InCharacterHandle);
}
void USeedExt_ActorManager::OnLoadCharacterFail(FSoftObjectPath InSoftObjectPath, FSeedExt_CharacterHandle InCharacterHandle)
{
	FSeedExt_ActorSpawnStatus* ActorSpawnStatus = CharacterSpawnStatus_List.Find(InCharacterHandle);
	if(ActorSpawnStatus == nullptr)
		return;

	ActorSpawnStatus->SpawnFailDelegate.ExecuteIfBound(InCharacterHandle, InSoftObjectPath);
	CharacterSpawnStatus_List.Remove(InCharacterHandle);
}

ASeedExt_CharacterInstance* USeedExt_ActorManager::SpawnCharacterActorInternal(UClass* InCharacterClass, const FSeedExt_ActorSpawnStatus& InActorSpawnStatus)
{
	if(InCharacterClass == nullptr)
		return nullptr;
	if(InCharacterClass->IsChildOf(ASeedExt_CharacterInstance::StaticClass()) == false)
		return nullptr;

	UWorld* World = GetWorld();
	if(World == nullptr)
		return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.ObjectFlags |= RF_Transient;

	const FTransform SpawnTransform(InActorSpawnStatus.SpawnRotation, InActorSpawnStatus.SpawnLocation, InActorSpawnStatus.SpawnScale);

	ASeedExt_CharacterInstance* SpawnedActor = World->SpawnActorDeferred<ASeedExt_CharacterInstance>(InCharacterClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if(SpawnedActor == nullptr)
		return nullptr;

	if(InActorSpawnStatus.bIsHideSpawn == true)
		SpawnedActor->SetActorHiddenInGame(true);

	UGameplayStatics::FinishSpawningActor(SpawnedActor, SpawnTransform);
	
	return SpawnedActor;
}

ASeedExt_CharacterInstance* USeedExt_ActorManager::GetCharacterInstance(const FSeedExt_CharacterHandle& InCharacterHandle)
{
	return SeedExt_GetSafeMapValue(CharacterInstance_List.Find(InCharacterHandle));
}
