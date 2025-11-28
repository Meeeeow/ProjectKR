// Fill out your copyright notice in the Description page of Project Settings.
#include "SeedExt_SubSystemCollector.h"

#include "SeedExt_SubSystemDefine.h"
#include "SeedExt_SubSystemInterface.h"

#include "SeedExt_Core/SeedExt_Util.h"

FSeedExt_SubSystemCollector* FSeedExt_SubSystemCollector::_pInstance = nullptr;

void FSeedExt_SubSystemCollector::_Init()
{
	check(_pInstance == nullptr);

	_pInstance = new FSeedExt_SubSystemCollector();
	check(_pInstance != nullptr);
}
void FSeedExt_SubSystemCollector::_Reset()
{
	check(_pInstance != nullptr);

	delete _pInstance;
	_pInstance = nullptr;
	check(_pInstance == nullptr);
}
void FSeedExt_SubSystemCollector::_Clear()
{
	
}

bool FSeedExt_SubSystemCollector::AddSubSystem(class ISeedExt_SubSystemInterface* pInSubSystem)
{
	check(pInSubSystem != nullptr);
	check(AllSubSystem_List.FindKey(pInSubSystem) == nullptr);

	//--------------------------------------------------// Check SubsystemRedirects
	if(IsAvailableSubSystem(pInSubSystem) == false)
		return false;

	//--------------------------------------------------//
	if(UWorld* SubSystemWorld = pInSubSystem->GetSubSystemWorld()){
		if(SubSystemWorld==nullptr || SeedExt_IsGameWorld(SubSystemWorld, true)==false)
			return false;
	}

	//--------------------------------------------------//
	AllSubSystem_List.Add(pInSubSystem->GetSubSystemOrder(), pInSubSystem);
	AllSubSystem_List.KeySort(TLess<int32>());

	if(EnumHasAnyFlags(pInSubSystem->GetSubSystemTickType(),ESeedExt_SubSystemTickType::System) == true){
		SystemTick_List.Add(pInSubSystem->GetSubSystemTickOrder(), pInSubSystem);
		SystemTick_List.KeySort(TLess<int32>());
	}
	if(EnumHasAnyFlags(pInSubSystem->GetSubSystemTickType(),ESeedExt_SubSystemTickType::World) == true){
		WorldTick_List.Add(pInSubSystem->GetSubSystemTickOrder(), pInSubSystem);
		WorldTick_List.KeySort(TLess<int32>());
	}

	return true;
}
bool FSeedExt_SubSystemCollector::RemoveSubSystem(class ISeedExt_SubSystemInterface* pInSubSystem)
{
	check(pInSubSystem != nullptr);

	if(AllSubSystem_List.FindKey(pInSubSystem) == nullptr)
		return false;

	if(UWorld *pSubSystemWorld = pInSubSystem->GetSubSystemWorld()){
		if(pSubSystemWorld==nullptr || SeedExt_IsGameWorld(pSubSystemWorld,false)==false)
			return false;
	}

	//--------------------------------------------------//
	AllSubSystem_List.Remove(pInSubSystem->GetSubSystemOrder(), pInSubSystem);
	AllSubSystem_List.KeySort(TLess<int32>());

	if(EnumHasAnyFlags(pInSubSystem->GetSubSystemTickType(),ESeedExt_SubSystemTickType::System) == true){
		SystemTick_List.Remove(pInSubSystem->GetSubSystemTickOrder(), pInSubSystem);
		SystemTick_List.KeySort(TLess<int32>());
	}
	if(EnumHasAnyFlags(pInSubSystem->GetSubSystemTickType(),ESeedExt_SubSystemTickType::World) == true){
		WorldTick_List.Remove(pInSubSystem->GetSubSystemTickOrder(), pInSubSystem);
		WorldTick_List.KeySort(TLess<int32>());
	}

	return true;
}

bool FSeedExt_SubSystemCollector::IsAvailableSubSystem(class ISeedExt_SubSystemInterface* pInSubSystem)
{
	return true;
}

bool FSeedExt_SubSystemCollector::Tick_System(float fInDeltaSeconds)
{
	return true;
}
bool FSeedExt_SubSystemCollector::Tick_World(class UWorld* pInWorld, float fInDeltaSeconds)
{
	return true;
}