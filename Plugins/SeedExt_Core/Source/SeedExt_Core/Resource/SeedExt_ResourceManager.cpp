// Fill out your copyright notice in the Description page of Project Settings.

#include "SeedExt_ResourceManager.h"

void FSeedExt_ResourceStatus::Init(const FSoftObjectPath& InResourcePath, UClass* InResourceUClass)
{
	ResourcePath = InResourcePath;
	ResourceUClass = InResourceUClass;
}
void FSeedExt_ResourceStatus::AddLoadCompleteDelegate(FSeedExt_ResourceLoadCompleteDelegate::FDelegate* InDelegate)
{
	if(InDelegate != nullptr)
	{
		ResourceLoadCompleteDelegate.Add(*InDelegate);
	}
}
void FSeedExt_ResourceStatus::AddLoadFailDelegate(FSeedExt_ResourceLoadFailDelegate::FDelegate* InDelegate)
{
	if(InDelegate != nullptr)
	{
		ResourceLoadFailDelegate.Add(*InDelegate);
	}
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

