// Fill out your copyright notice in the Description page of Project Settings.

#include "SeedExt_ResourceManager.h"

#include "SeedExt_Core/SeedExt_Util.h"

#include "UObject/GarbageCollectionSchema.h"

void FSeedExt_ResourceStatus::Init(const FSoftObjectPath& InResourcePath, UClass* InResourceUClass, ESeedExt_ResourceType InResourceType, ESeedExt_ResourceLoadOrderType InResourceLoadOrderType)
{
	ResourcePath = InResourcePath;
	ResourceUClass = InResourceUClass;
	ResourceType = InResourceType;
	ResourceLoadOrderType = InResourceLoadOrderType;

	bLoadResult.Reset();

	ResourceObject = nullptr;
	ReferenceCount = 0;
	GiveCount = 0;
	ReturnCount = 0;
	UnloadDelayTime = FSeedExt_ResourceUtil::GetDefaultUnloadDelay(InResourceType);
	RemainUnloadTime = UnloadDelayTime;
}
void FSeedExt_ResourceStatus::SetLoadedObject(UObject* InObject)
{
	ResourceObject = InObject;
	bLoadResult = TOptional<bool>(ResourceObject != nullptr);
	RemainUnloadTime = UnloadDelayTime;
}
void FSeedExt_ResourceStatus::MarkLoadFailed()
{
	bLoadResult = TOptional<bool>(false);
}

void FSeedExt_ResourceStatus::OnReference()
{
	++ReferenceCount;
	++GiveCount;
	RemainUnloadTime = UnloadDelayTime;
}
void FSeedExt_ResourceStatus::OnReturn()
{
	--ReferenceCount;
	check(ReferenceCount >= 0);
	++ReturnCount;

	RemainUnloadTime = UnloadDelayTime;
}
bool FSeedExt_ResourceStatus::TickUnload(float InDeltaTime)
{
	if(ReferenceCount > 0)
		return true;

	RemainUnloadTime -= InDeltaTime;
	return (RemainUnloadTime > 0.0f);
}

void FSeedExt_ResourceStatus::AddLoadCompleteDelegate(FSeedExt_ResourceLoadCompleteDelegate::FDelegate* InDelegate)
{
	if(InDelegate != nullptr)
		ResourceLoadCompleteDelegate.Add(*InDelegate);
}
void FSeedExt_ResourceStatus::AddLoadFailDelegate(FSeedExt_ResourceLoadFailDelegate::FDelegate* InDelegate)
{
	if(InDelegate != nullptr)
		ResourceLoadFailDelegate.Add(*InDelegate);
}
void FSeedExt_ResourceStatus::ClearLoadResultDelegate()
{
	ResourceLoadCompleteDelegate.Clear();
	ResourceLoadFailDelegate.Clear();
}

void FSeedExt_ResourceStatus::ExecuteLoadComplete()
{
	ResourceLoadCompleteDelegate.Broadcast(ResourcePath, ResourceObject, ResourceUClass);
}
void FSeedExt_ResourceStatus::ExecuteLoadFail()
{
	ResourceLoadFailDelegate.Broadcast(ResourcePath);
}

void USeedExt_ResourceManager::AddReferencedObjets(UObject* InThis, FReferenceCollector& InCollector)
{
	USeedExt_ResourceManager* This = CastChecked<USeedExt_ResourceManager>(InThis);
	for(const TPair<FSoftObjectPath, FSeedExt_ResourceStatus*>& Pair : This->ResourceStatus_List)
	{
		if(Pair.Value!=nullptr && Pair.Value->ResourceObject!=nullptr)
			InCollector.AddReferencedObject(Pair.Value->ResourceObject, InThis);
	}

	Super::AddReferencedObjects(InThis, InCollector);
}

TArray<FSeedExt_ResourceStatus*>& USeedExt_ResourceManager::GetPendingList(ESeedExt_ResourceLoadOrderType InResourceLoadOrderType)
{
	switch(InResourceLoadOrderType)
	{
		case ESeedExt_ResourceLoadOrderType::Normal:	return PendingNormalLoad_List;
		case ESeedExt_ResourceLoadOrderType::Priority:	return PendingPriorityLoad_List;
		case ESeedExt_ResourceLoadOrderType::Previous:	return PendingPreviousLoad_List;
	}
	static TArray<FSeedExt_ResourceStatus*> PendingDummy_List;
	
	return PendingDummy_List;
}
TSharedPtr<struct FStreamableHandle>& USeedExt_ResourceManager::GetStreamableHandle(ESeedExt_ResourceLoadOrderType InResourceLoadOrderType)
{
	switch(InResourceLoadOrderType)
	{
		case ESeedExt_ResourceLoadOrderType::Normal:	return StreamableHandle_Normal;
		case ESeedExt_ResourceLoadOrderType::Priority:	return StreamableHandle_Priority;
		case ESeedExt_ResourceLoadOrderType::Previous:	return StreamableHandle_Previous;
	}

	static TSharedPtr<FStreamableHandle> StreamableHandle_Dummy;
	return StreamableHandle_Dummy;
}

void USeedExt_ResourceManager::Init()
{
	Super::Init();

	ResourceStatus_List.Empty();
	PendingNormalLoad_List.Empty(); StreamableHandle_Normal.Reset();
	PendingPriorityLoad_List.Empty(); StreamableHandle_Priority.Reset();
	PendingPreviousLoad_List.Empty(); StreamableHandle_Previous.Reset();
	AlreadyResourceLoadFail_List.Empty();
}
void USeedExt_ResourceManager::Reset()
{
	UnloadAll();

	ResourceStatus_List.Empty();
	PendingNormalLoad_List.Empty();
	if(StreamableHandle_Normal.IsValid() == true)
	{
		StreamableHandle_Normal->CancelHandle();
		StreamableHandle_Normal.Reset();
	}
	PendingPriorityLoad_List.Empty();
	if(StreamableHandle_Priority.IsValid() == true)
	{
		StreamableHandle_Priority->CancelHandle();
		StreamableHandle_Priority.Reset();
	}
	PendingPreviousLoad_List.Empty();
	if(StreamableHandle_Previous.IsValid() == true)
	{
		StreamableHandle_Previous->CancelHandle();
		StreamableHandle_Previous.Reset();
	}
	
	AlreadyResourceLoadFail_List.Empty();

	Super::Reset();
}

bool USeedExt_ResourceManager::Tick_System(float InDeltaSeconds)
{
	TickUnloadCheck(InDeltaSeconds);
	
	return true;
}

FSeedExt_ResourceStatus* USeedExt_ResourceManager::GetOrCreateStatus(const FSoftObjectPath& InResourcePath, UClass* InUClass, ESeedExt_ResourceType InResourceType, ESeedExt_ResourceLoadOrderType InResourceLoadOrderType)
{
	if(FSeedExt_ResourceStatus* FoundStatus = SeedExt_GetSafeMapValue(ResourceStatus_List.Find(InResourcePath)))
		return FoundStatus;

	FSeedExt_ResourceStatus* NewStatus = new FSeedExt_ResourceStatus;
	NewStatus->Init(InResourcePath, InUClass, InResourceType, InResourceLoadOrderType);
	ResourceStatus_List.Emplace(InResourcePath, NewStatus);
	return NewStatus;
}
FSeedExt_ResourceStatus* USeedExt_ResourceManager::GetStatus(const FSoftObjectPath& InResourcePath) const
{
	if(FSeedExt_ResourceStatus* FoundStatus = SeedExt_GetSafeMapConstValue(ResourceStatus_List.Find(InResourcePath)))
		return FoundStatus;

	return nullptr;
}
void USeedExt_ResourceManager::DestroyStatus(FSeedExt_ResourceStatus* InStatus)
{
	if(InStatus == nullptr)
		return;

	ResourceStatus_List.Remove(InStatus->ResourcePath);
	delete InStatus;
	InStatus = nullptr;
}

void USeedExt_ResourceManager::StartAsyncLoad(ESeedExt_ResourceLoadOrderType InResourceLoadOrderType)
{
	switch(InResourceLoadOrderType)
	{
		case ESeedExt_ResourceLoadOrderType::Normal:	break;
		case ESeedExt_ResourceLoadOrderType::Priority:	break;
		case ESeedExt_ResourceLoadOrderType::Previous:	break;
		default:										return;
	}
	
	TSharedPtr<FStreamableHandle>& OrderStreamableHandle = GetStreamableHandle(InResourceLoadOrderType);
	TArray<FSeedExt_ResourceStatus*>& OrderPending_List = GetPendingList(InResourceLoadOrderType);

	if(OrderStreamableHandle.IsValid() == true)
		return;
	if(OrderPending_List.Num() == 0)
		return;
	if(IsProcessableLoad(InResourceLoadOrderType) == false)
		return;
	
	const int32 MaxRequest = GetLoadRequestMaxValue(InResourceLoadOrderType);
	const int32 RequestCount = ( MaxRequest>0 ) ? FMath::Min(MaxRequest, OrderPending_List.Num()) : OrderPending_List.Num();

	TArray<FSoftObjectPath> ResourcePath_List;
	ResourcePath_List.Reserve(RequestCount);

	for(int32 Index=0; Index<RequestCount; ++Index)
	{
		if(OrderPending_List.IsValidIndex(Index) == false)
			continue;
		
		FSeedExt_ResourceStatus* ResourceStatus = OrderPending_List[Index];
		if(ResourceStatus != nullptr && ResourceStatus->IsLoadComplete()==false)
			ResourcePath_List.Add(ResourceStatus->ResourcePath);
	}

	if(ResourcePath_List.IsEmpty() == true)
	{
		OrderPending_List.Reset();
		return;
	}

	TAsyncLoadPriority Priority = FStreamableManager::DefaultAsyncLoadPriority;
	switch(InResourceLoadOrderType)
	{
		case ESeedExt_ResourceLoadOrderType::Normal:	{ Priority = FStreamableManager::DefaultAsyncLoadPriority;	break; }
		case ESeedExt_ResourceLoadOrderType::Priority:	{ Priority = FStreamableManager::AsyncLoadHighPriority;		break; }
		case ESeedExt_ResourceLoadOrderType::Previous:	{ Priority = 0;												break; }
	}
	
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	
	OrderStreamableHandle = StreamableManager.RequestAsyncLoad(ResourcePath_List, FStreamableDelegate::CreateUObject(this, &USeedExt_ResourceManager::OnAsyncLoadComplete, InResourceLoadOrderType), Priority, false);
}
void USeedExt_ResourceManager::OnAsyncLoadComplete(ESeedExt_ResourceLoadOrderType InResourceLoadOrderType)
{
	switch(InResourceLoadOrderType)
	{
		case ESeedExt_ResourceLoadOrderType::Normal:	break;
		case ESeedExt_ResourceLoadOrderType::Priority:	break;
		case ESeedExt_ResourceLoadOrderType::Previous:	break;
		default:										return;
	}

	TSharedPtr<FStreamableHandle>& OrderStreamableHandle = GetStreamableHandle(InResourceLoadOrderType);
	TArray<FSeedExt_ResourceStatus*>& OrderPending_List = GetPendingList(InResourceLoadOrderType);

	const int32 MaxRequest = GetLoadRequestMaxValue(InResourceLoadOrderType);
	const int32 ProcessedCount = ( MaxRequest>0 ) ? FMath::Min(MaxRequest, OrderPending_List.Num()) : OrderPending_List.Num();

	for(int32 Index=0; Index<ProcessedCount; ++Index)
	{
		FSeedExt_ResourceStatus* Status = OrderPending_List[Index];
		if(Status == nullptr)
			continue;

		if(Status->IsLoadComplete() == true)
		{
			Status->ClearLoadResultDelegate();
			continue;
		}

		UObject* LoadedObject = Status->ResourcePath.ResolveObject();
		if(LoadedObject != nullptr)
		{
			Status->SetLoadedObject(LoadedObject);
			Status->ExecuteLoadComplete();
		}
		else
		{
			Status->MarkLoadFailed();
			AlreadyResourceLoadFail_List.Add(Status->ResourcePath);
			Status->ExecuteLoadFail();
		}

		Status->ClearLoadResultDelegate();
	}

	OrderPending_List.RemoveAt(0, ProcessedCount, false);
	OrderStreamableHandle.Reset();

	if(OrderPending_List.Num() > 0)
		StartAsyncLoad(InResourceLoadOrderType);

	if(InResourceLoadOrderType==ESeedExt_ResourceLoadOrderType::Priority || InResourceLoadOrderType==ESeedExt_ResourceLoadOrderType::Normal)
		StartAsyncLoad(ESeedExt_ResourceLoadOrderType::Previous);
}
void USeedExt_ResourceManager::TickUnloadCheck(float InDeltaSecond)
{
	static TArray<FSeedExt_ResourceStatus*> DestroyStatus_List;
	for(const TPair<FSoftObjectPath, FSeedExt_ResourceStatus*>& Pair : ResourceStatus_List)
	{
		FSeedExt_ResourceStatus* Status = Pair.Value;
		if(Status == nullptr)
			continue;
		if(Status->IsLoadComplete()==false)
			continue;
		if(Status->TickUnload(InDeltaSecond)==false)
			DestroyStatus_List.Add(Status);
	}

	for(FSeedExt_ResourceStatus* Status : DestroyStatus_List)
	{
		DestroyStatus(Status);
	}

	DestroyStatus_List.Empty();
}

bool USeedExt_ResourceManager::IsProcessableLoad(ESeedExt_ResourceLoadOrderType InResourceLoadOrderType) const
{
	if(InResourceLoadOrderType == ESeedExt_ResourceLoadOrderType::Previous)
	{
		if(StreamableHandle_Priority.IsValid() == true)
			return false;
		if(StreamableHandle_Normal.IsValid() == true)
			return false;
	}

	return true;
}
int32 USeedExt_ResourceManager::GetLoadRequestMaxValue(ESeedExt_ResourceLoadOrderType InResourceLoadOrderType)
{
	switch(InResourceLoadOrderType)
	{
		case ESeedExt_ResourceLoadOrderType::Normal:	return LoadRequestMax_Normal;
		case ESeedExt_ResourceLoadOrderType::Priority:	return LoadRequestMax_Priority;
		case ESeedExt_ResourceLoadOrderType::Previous:	return LoadRequestMax_Previous;
		default:										return 0;
	}
}

UObject* USeedExt_ResourceManager::GetResource(const FSoftObjectPath& InResourcePath, bool bInIsUpdateReferenceCount)
{
	FSeedExt_ResourceStatus* Status = GetStatus(InResourcePath);
	if(Status==nullptr || Status->IsLoadComplete()==false)
		return nullptr;

	if(bInIsUpdateReferenceCount==true)
		Status->OnReference();

	return Status->ResourceObject;
}
bool USeedExt_ResourceManager::ReturnResource(const FSoftObjectPath& InResourcePath, float InAdditionalDelay)
{
	FSeedExt_ResourceStatus* Status = GetStatus(InResourcePath);
	if(Status == nullptr)
		return false;

	Status->OnReturn();
	Status->RemainUnloadTime += InAdditionalDelay;
	return true;
}
bool USeedExt_ResourceManager::IsRegistedResource(const FSoftObjectPath& InResourcePath) const
{
	const FSeedExt_ResourceStatus* Status = GetStatus(InResourcePath);

	return (Status!=nullptr && Status->IsLoadComplete()==true);
}
bool USeedExt_ResourceManager::IsLoading(const FSoftObjectPath& InResourcePath) const
{
	if(InResourcePath.IsValid() == false)
		return ( (StreamableHandle_Normal.IsValid()==true) || (StreamableHandle_Priority.IsValid()==true) || (StreamableHandle_Previous.IsValid()==true) );

	const FSeedExt_ResourceStatus* Status = GetStatus(InResourcePath);
	if(Status == nullptr)
		return false;

	return Status->IsLoadPending();
}
bool USeedExt_ResourceManager::IsLoadFail(const FSoftObjectPath& InResourcePath) const
{
	return AlreadyResourceLoadFail_List.Contains(InResourcePath);
}

void USeedExt_ResourceManager::UnloadObject(const FSoftObjectPath& InResourcePath)
{
	if(FSeedExt_ResourceStatus* Status = GetStatus(InResourcePath))
		DestroyStatus(Status);
}

void USeedExt_ResourceManager::UnloadAll()
{
	TArray<FSeedExt_ResourceStatus*> UnloadStatus_List;
	UnloadStatus_List.Reserve(ResourceStatus_List.Num());

	for(const TPair<FSoftObjectPath, FSeedExt_ResourceStatus*>& Pair : ResourceStatus_List)
	{
		UnloadStatus_List.Emplace(Pair.Value);
	}

	for(FSeedExt_ResourceStatus* UnloadStatus : UnloadStatus_List)
	{
		DestroyStatus(UnloadStatus);
	}

	UnloadStatus_List.Empty();
	ResourceStatus_List.Empty();
}

