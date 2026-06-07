// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/AssetManager.h"

#include "SeedExt_Core/SubSystem/SeedExt_SubSystemBase.h"
#include "SeedExt_Core/Resource/SeedExt_ResourceDefine.h"
#include "SeedExt_ResourceManager.generated.h"

#define SeedExt_GetResourceManager(World) FSeedExt_SubSystemCollector::_GetSubSystem<USeedExt_ResourceManager>(World)
#define SeedExt_HasResourceManager(World) FSeedExt_SubSystemCollector::_HasSubSystem<USeedExt_ResourceManager>(World)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SEEDEXT_CORE_API FSeedExt_ResourceStatus
{
	friend class USeedExt_ResourceManager;

public:
	FSeedExt_ResourceStatus() = default;
	~FSeedExt_ResourceStatus() = default;

private:
	FSoftObjectPath ResourcePath = FSoftObjectPath();
	UClass* ResourceUClass = nullptr;
	ESeedExt_ResourceType ResourceType = ESeedExt_ResourceType::None;

	ESeedExt_ResourceLoadOrderType ResourceLoadOrderType = ESeedExt_ResourceLoadOrderType::None;
	TOptional<bool> bLoadResult = TOptional<bool>();
	
	TObjectPtr<UObject> ResourceObject = nullptr;
	int32 ReferenceCount = 0;
	int32 GiveCount = 0;
	int32 ReturnCount = 0;
	float UnloadDelayTime = SEEDEXT_RESOURCE_DELAY_DEFAULT;
	float RemainUnloadTime = 0.0f;
	
	FSeedExt_ResourceLoadCompleteDelegate ResourceLoadCompleteDelegate;
	FSeedExt_ResourceLoadFailDelegate ResourceLoadFailDelegate;

protected:
	void Init(const FSoftObjectPath& InResourcePath, UClass* InResourceUClass, ESeedExt_ResourceType InResourceType, ESeedExt_ResourceLoadOrderType InResourceLoadOrderType);
	void SetLoadedObject(UObject* InObject);
	void MarkLoadFailed();

	void OnReference();
	void OnReturn();
	bool TickUnload(float InDeltaTime);
	
	void AddLoadCompleteDelegate(FSeedExt_ResourceLoadCompleteDelegate::FDelegate* InDelegate);
	void AddLoadFailDelegate(FSeedExt_ResourceLoadFailDelegate::FDelegate* InDelegate);
	void ClearLoadResultDelegate();

	void ExecuteLoadComplete();
	void ExecuteLoadFail();

public:
	FORCEINLINE const FSoftObjectPath& GetResourcePath() const { return ResourcePath; }
	FORCEINLINE const UClass* GetResourceUClass() const { return ResourceUClass; }
	FORCEINLINE ESeedExt_ResourceType GetResourceType() const { return ResourceType; }
	FORCEINLINE UObject* GetResourceObject() const { return ResourceObject; }
	FORCEINLINE TOptional<bool> GetLoadResult() const { return bLoadResult; }
	FORCEINLINE int32 GetReferCount() const { return ReferenceCount; }

	FORCEINLINE bool IsLoadPending() const { return bLoadResult.IsSet()==false; }
	FORCEINLINE bool IsLoadComplete() const { return bLoadResult.IsSet()==true && bLoadResult.GetValue()==true; }
	FORCEINLINE bool IsLoadFail() const { return bLoadResult.IsSet()==true && bLoadResult.GetValue()==false; }
};


UCLASS()
class SEEDEXT_CORE_API USeedExt_ResourceManager : public USeedExt_EngineSubSystem
{
	GENERATED_BODY()
	
public:
	static void AddReferencedObjets(UObject* InThis, FReferenceCollector& InCollector);
	
private:
	TMap<FSoftObjectPath, FSeedExt_ResourceStatus*> ResourceStatus_List;

	static constexpr int32 LoadRequestMax_Normal = -1;
	static constexpr int32 LoadRequestMax_Priority = -1;
	static constexpr int32 LoadRequestMax_Previous = 10;

	TArray<FSeedExt_ResourceStatus*> PendingNormalLoad_List;		TSharedPtr<struct FStreamableHandle> StreamableHandle_Normal;
	TArray<FSeedExt_ResourceStatus*> PendingPriorityLoad_List;		TSharedPtr<struct FStreamableHandle> StreamableHandle_Priority;
	TArray<FSeedExt_ResourceStatus*> PendingPreviousLoad_List;		TSharedPtr<struct FStreamableHandle> StreamableHandle_Previous;
	
	TSet<FSoftObjectPath> AlreadyResourceLoadFail_List;

	TArray<FSeedExt_ResourceStatus*>& GetPendingList(ESeedExt_ResourceLoadOrderType InResourceLoadOrderType);
	TSharedPtr<struct FStreamableHandle>& GetStreamableHandle(ESeedExt_ResourceLoadOrderType InResourceLoadOrderType);
	
protected:
	virtual void Init() override;
	virtual void Reset() override;
	virtual bool Tick_System(float InDeltaSeconds) override;

	FSeedExt_ResourceStatus* GetOrCreateStatus(const FSoftObjectPath& InResourcePath, UClass* InUClass, ESeedExt_ResourceType InResourceType, ESeedExt_ResourceLoadOrderType InResourceLoadOrderType);
	FSeedExt_ResourceStatus* GetStatus(const FSoftObjectPath& InResourcePath) const;
	void DestroyStatus(FSeedExt_ResourceStatus* InStatus);

	void StartAsyncLoad(ESeedExt_ResourceLoadOrderType InResourceLoadOrderType);
	void OnAsyncLoadComplete(ESeedExt_ResourceLoadOrderType InResourceLoadOrderType);
	void TickUnloadCheck(float InDeltaSecond);

	bool IsProcessableLoad(ESeedExt_ResourceLoadOrderType InResourceLoadOrderType) const;
	static int32 GetLoadRequestMaxValue(ESeedExt_ResourceLoadOrderType InResourceLoadOrderType);
	
public:
	template<typename T> T* ImmediatelyLoadObject(const FSoftObjectPath&& InResourcePath, ESeedExt_ResourceType InResourceType);
	template<typename T> void LoadObject(const FSoftObjectPath& InResourcePath, ESeedExt_ResourceType InResourceType, ESeedExt_ResourceLoadOrderType InResourceLoadOrderType,
							  FSeedExt_ResourceLoadCompleteDelegate::FDelegate* InLoadCompleteDelegate = nullptr, FSeedExt_ResourceLoadFailDelegate::FDelegate* InLoadFailDelegate = nullptr);
	template<typename T> void PriorityLoadObject(const FSoftObjectPath& InResourcePath, ESeedExt_ResourceType InResourceType, ESeedExt_ResourceLoadOrderType InResourceLoadOrderType,
							  FSeedExt_ResourceLoadCompleteDelegate::FDelegate* InLoadCompleteDelegate = nullptr, FSeedExt_ResourceLoadFailDelegate::FDelegate* InLoadFailDelegate = nullptr);
	template<typename T> void PreviousLoadObject(const FSoftObjectPath& InResourcePath, ESeedExt_ResourceType InResourceType, ESeedExt_ResourceLoadOrderType InResourceLoadOrderType,
							  FSeedExt_ResourceLoadCompleteDelegate::FDelegate* InLoadCompleteDelegate = nullptr, FSeedExt_ResourceLoadFailDelegate::FDelegate* InLoadFailDelegate = nullptr);
	
private:
	template<typename T> void LoadObject_Inner(const FSoftObjectPath& InResourcePath, ESeedExt_ResourceType InResourceType, ESeedExt_ResourceLoadOrderType InResourceLoadOrderType,
							  FSeedExt_ResourceLoadCompleteDelegate::FDelegate* InLoadCompleteDelegate, FSeedExt_ResourceLoadFailDelegate::FDelegate* InLoadFailDelegate);
	
public:
	UObject* GetResource(const FSoftObjectPath& InResourcePath, bool bInIsUpdateReferenceCount = true);
	bool ReturnResource(const FSoftObjectPath& InResourcePath, float InAdditionalDelay = 0.0f);
	bool IsRegistedResource(const FSoftObjectPath& InResourcePath) const;
	bool IsLoading(const FSoftObjectPath& InResourcePath = FSoftObjectPath()) const;
	bool IsLoadFail(const FSoftObjectPath& InResourcePath) const;

	void UnloadObject(const FSoftObjectPath& InResourcePath);

private:
	void UnloadAll();
};

template <typename T>
T* USeedExt_ResourceManager::ImmediatelyLoadObject(const FSoftObjectPath&& InResourcePath, ESeedExt_ResourceType InResourceType)
{
	if(FSeedExt_ResourceUtil::IsValidResourcePath(InResourcePath) == false)
		return nullptr;

	if(AlreadyResourceLoadFail_List.Contains(InResourcePath) == true)
		return nullptr;

	UClass* StaticUClass = T::StaticClass();
	check(StaticUClass != nullptr);

	if(FSeedExt_ResourceStatus* Status = GetStatus(InResourcePath))
	{
		if(Status->IsLoadComplete() == TOptional<bool>(true))
		{
			Status->OnReference();
			return Cast<T>(Status->GetResourceObject());
		}
	}

	UObject* SyncLoadObject = UAssetManager::GetStreamableManager().LoadSynchronous(InResourcePath);
	if(SyncLoadObject!=nullptr && SyncLoadObject->GetClass()->IsChildOf(StaticUClass)==false)
		SyncLoadObject = StaticLoadObject(StaticUClass, nullptr, *(InResourcePath.ToString()));

	if(SyncLoadObject == nullptr)
	{
		AlreadyResourceLoadFail_List.Add(InResourcePath);
		return nullptr;
	}

	FSeedExt_ResourceStatus* Status = GetOrCreateStatus(InResourcePath, StaticUClass, InResourceType, ESeedExt_ResourceLoadOrderType::None);
	Status->SetLoadedObject(SyncLoadObject);
	Status->OnReference();
	return Cast<T>(SyncLoadObject);
}

template <typename T>
void USeedExt_ResourceManager::LoadObject(const FSoftObjectPath& InResourcePath, ESeedExt_ResourceType InResourceType, ESeedExt_ResourceLoadOrderType InResourceLoadOrderType, FSeedExt_ResourceLoadCompleteDelegate::FDelegate* InLoadCompleteDelegate, FSeedExt_ResourceLoadFailDelegate::FDelegate* InLoadFailDelegate)
{
	LoadObject_Inner<T>(InResourcePath, InResourceType, ESeedExt_ResourceLoadOrderType::Normal, InLoadCompleteDelegate, InLoadFailDelegate);
}
template <typename T>
void USeedExt_ResourceManager::PriorityLoadObject(const FSoftObjectPath& InResourcePath, ESeedExt_ResourceType InResourceType, ESeedExt_ResourceLoadOrderType InResourceLoadOrderType, FSeedExt_ResourceLoadCompleteDelegate::FDelegate* InLoadCompleteDelegate, FSeedExt_ResourceLoadFailDelegate::FDelegate* InLoadFailDelegate)
{
	LoadObject_Inner<T>(InResourcePath, InResourceType, ESeedExt_ResourceLoadOrderType::Priority, InLoadCompleteDelegate, InLoadFailDelegate);
}
template <typename T>
void USeedExt_ResourceManager::PreviousLoadObject(const FSoftObjectPath& InResourcePath, ESeedExt_ResourceType InResourceType, ESeedExt_ResourceLoadOrderType InResourceLoadOrderType, FSeedExt_ResourceLoadCompleteDelegate::FDelegate* InLoadCompleteDelegate, FSeedExt_ResourceLoadFailDelegate::FDelegate* InLoadFailDelegate)
{
	LoadObject_Inner<T>(InResourcePath, InResourceType, ESeedExt_ResourceLoadOrderType::Previous, InLoadCompleteDelegate, InLoadFailDelegate);
}

template <typename T>
void USeedExt_ResourceManager::LoadObject_Inner(const FSoftObjectPath& InResourcePath, ESeedExt_ResourceType InResourceType, ESeedExt_ResourceLoadOrderType InResourceLoadOrderType, FSeedExt_ResourceLoadCompleteDelegate::FDelegate* InLoadCompleteDelegate, FSeedExt_ResourceLoadFailDelegate::FDelegate* InLoadFailDelegate)
{
	if(FSeedExt_ResourceUtil::IsValidResourcePath(InResourcePath) == false)
		return;

	UClass* StaticUClass = T::StaticClass();
	check(StaticUClass != nullptr);
	FSeedExt_ResourceStatus* Status = GetStatus(InResourcePath); 
	if(Status!=nullptr && Status->IsLoadComplete()==TOptional<bool>(true))
	{
		if(InLoadCompleteDelegate != nullptr)
			Status->AddLoadCompleteDelegate(InLoadCompleteDelegate);
		if(InLoadFailDelegate != nullptr)
			Status->AddLoadFailDelegate(InLoadFailDelegate);

		Status->ExecuteLoadComplete();
		Status->ClearLoadResultDelegate();
		return;
	}

	if(AlreadyResourceLoadFail_List.Contains(InResourcePath) == true)
	{
		if(InLoadFailDelegate != nullptr)
		{
			FSeedExt_ResourceStatus LoadFailDelegator;
			LoadFailDelegator.Init(InResourcePath, StaticUClass, InResourceType, InResourceLoadOrderType);
			LoadFailDelegator.AddLoadFailDelegate(InLoadFailDelegate);
			LoadFailDelegator.ExecuteLoadFail();
		}
		return;
	}

	if(Status == nullptr)
	{
		Status = GetOrCreateStatus(InResourcePath, StaticUClass, InResourceType, InResourceLoadOrderType);
		switch(InResourceLoadOrderType)
		{
			case ESeedExt_ResourceLoadOrderType::Normal:	{ PendingNormalLoad_List.Emplace(Status);	break; }
			case ESeedExt_ResourceLoadOrderType::Priority:	{ PendingPriorityLoad_List.Emplace(Status);	break; }
			case ESeedExt_ResourceLoadOrderType::Previous:	{ PendingPreviousLoad_List.Emplace(Status);	break; }
		}

		if(InLoadCompleteDelegate != nullptr)
			Status->AddLoadCompleteDelegate(InLoadCompleteDelegate);
		if(InLoadFailDelegate != nullptr)
			Status->AddLoadFailDelegate(InLoadFailDelegate);
	}

	StartAsyncLoad(InResourceLoadOrderType);
}


