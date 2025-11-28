// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SeedExt_SubSystemBase.h"

class SEEDEXT_CORE_API FSeedExt_SubSystemCollector
{
public:
	FORCEINLINE static FSeedExt_SubSystemCollector* _GetInstance()
	{
		check(_pInstance != nullptr);
		return _pInstance;
	}
	FORCEINLINE static bool _HasInstance()
	{
		return (_pInstance!=nullptr) ? true : false;
	}

	static void _Init();
	static void _Reset();
	static void _Clear();

protected:
	static FSeedExt_SubSystemCollector* _pInstance;

	TMap<UClass*, TWeakObjectPtr<USubsystem>> SubSystem_List;
	UObject* LastSubSystemWorld = nullptr;

	FTSTicker::FDelegateHandle TickDelegateHandle;
	FDelegateHandle PostLoadMapWithWorldDelegateHandle;
	FDelegateHandle WorldInitDelegateHandle;
	FDelegateHandle WorldBeginTearDownDelegateHandle;
	FDelegateHandle EnginePreExitDelegateHandle;

	TMultiMap<int32, class ISeedExt_SubSystemInterface*> AllSubSystem_List;	// <SubSystemOrder, SubSystem*>

public:
	bool AddSubSystem(class ISeedExt_SubSystemInterface* InSubSystem);
	bool RemoveSubSystem(class ISeedExt_SubSystemInterface* InSubSystem);

protected:
	bool IsAvailableSubSystem(class ISeedExt_SubSystemInterface* InSubSystem);

	TMultiMap<int32, class ISeedExt_SubSystemInterface*> SystemTick_List;
	TMultiMap<int32, class ISeedExt_SubSystemInterface*> WorldTick_List;

	bool Tick_System(float fInDeltaSeconds);
	bool Tick_World(class UWorld* pInWorld, float fInDeltaSeconds);

	TMap<UClass*,TWeakObjectPtr<USubsystem>> LastSubSystem_List;
	TMap<UClass*,TWeakObjectPtr<USubsystem>> LastSubSystemForWorld_List;

public:
	template <class TSubSystem>
	static TSubSystem* _GetSubSystem(UObject* pInWorldContextObject = nullptr)
	{
		if(_pInstance != nullptr){
			UWorld* World = (pInWorldContextObject!=nullptr) ? ((pInWorldContextObject->IsA(UWorld::StaticClass())==true) ? Cast<UWorld>(pInWorldContextObject) : pInWorldContextObject->GetWorld()) : nullptr;
			if(World!=nullptr && World!=_pInstance->LastSubSystemWorld)
				_pInstance->LastSubSystemWorld = World;

			if(pInWorldContextObject == nullptr){
				TWeakObjectPtr<USubsystem>& LastSubsystem = _pInstance->LastSubSystem_List.FindOrAdd(TSubSystem::StaticClass());
				if(LastSubsystem.IsValid() == true)
					return Cast<TSubSystem>(LastSubsystem.Get());
			}
			else{
				TWeakObjectPtr<USubsystem>& LastSubsystem = _pInstance->LastSubSystemForWorld_List.FindOrAdd(TSubSystem::StaticClass());
				if(LastSubsystem.IsValid() == true)
					return Cast<TSubSystem>(LastSubsystem.Get());
			}
		}

		//--------------------------------------------------//
		UClass* SubsystemClass = TSubSystem::StaticClass();

		TSubSystem* FoundSubSystem = nullptr;
		if constexpr (std::is_base_of_v<USeedExt_EngineSubSystem,TSubSystem> == true)
			FoundSubSystem = USeedExt_EngineSubSystem::_GetSubSystem<TSubSystem>(SubsystemClass);
		else if constexpr (std::is_base_of_v<USeedExt_GameInstanceSubSystem,TSubSystem> == true)
			FoundSubSystem = USeedExt_GameInstanceSubSystem::_GetSubsystem<TSubSystem>(pInWorldContextObject, SubsystemClass);
		//else if constexpr (std::is_base_of_v<USeedExt_WorldSubsystem,TSubSystem> == true)
		//	FoundSubSystem = USeedExt_WorldSubsystem::_GetSubsystem<TSubSystem>(pInWorldContextObject, SubsystemClass);
		//else if constexpr (std::is_base_of_v<USeedExt_LocalPlayerSubsystem,TSubSystem> == true)
		//	FoundSubSystem = USeedExt_LocalPlayerSubsystem::_GetSubsystem<TSubSystem>(pInWorldContextObject, SubsystemClass);
		//if(FoundSubSystem==nullptr || FoundSubSystem->IsInitiated()==false)
		//	return nullptr;

		//--------------------------------------------------//
		if(_pInstance != nullptr){
			if(pInWorldContextObject == nullptr)
				_pInstance->LastSubSystem_List[TSubSystem::StaticClass()] = FoundSubSystem;
			else
				_pInstance->LastSubSystemForWorld_List[TSubSystem::StaticClass()] = FoundSubSystem;
		}

		return FoundSubSystem;
	}
	template<class TSubSystem>
	static bool _HasSubSystem(UWorld* pInWorldContextObject = nullptr)
	{
		if(_GetSubSystem<TSubSystem>(pInWorldContextObject) == nullptr)
			return false;

		return true;
	}
};
