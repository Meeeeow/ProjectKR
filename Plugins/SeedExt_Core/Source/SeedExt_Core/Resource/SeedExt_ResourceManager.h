// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SeedExt_Core/SubSystem/SeedExt_SubSystemBase.h"
#include "SeedExt_ResourceManager.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FSeedExt_ResourceLoadCompleteDelegate, FSoftObjectPath, UObject*, UClass*);
DECLARE_MULTICAST_DELEGATE_OneParam(FSeedExt_ResourceLoadFailDelegate, FSoftObjectPath);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SEEDEXT_CORE_API FSeedExt_ResourceStatus
{
	friend class USeedExt_ResourceManager;

private:
	FSoftObjectPath ResourcePath = FSoftObjectPath();
	UClass* ResourceUClass = nullptr;
	FSeedExt_ResourceLoadCompleteDelegate ResourceLoadCompleteDelegate;
	FSeedExt_ResourceLoadFailDelegate ResourceLoadFailDelegate;

	TObjectPtr<UObject> ResourceObject = nullptr;

protected:
	void Init(const FSoftObjectPath& InResourcePath, UClass* InResourceUClass);
	void AddLoadCompleteDelegate(FSeedExt_ResourceLoadCompleteDelegate::FDelegate* InDelegate);
	void AddLoadFailDelegate(FSeedExt_ResourceLoadFailDelegate::FDelegate* InDelegate);
	void ClearLoadResultDelegate();

	void ExecuteLoadComplete();
	void ExecuteLoadFail();
};


UCLASS()
class SEEDEXT_CORE_API USeedExt_ResourceManager : public USeedExt_EngineSubSystem
{
	GENERATED_BODY()
	
};

