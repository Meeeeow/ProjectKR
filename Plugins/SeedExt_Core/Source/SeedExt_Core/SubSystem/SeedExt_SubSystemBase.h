// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SeedExt_SubSystemDefine.h"
#include "SeedExt_SubSystemInterface.h"

#include "UObject/Object.h"
#include "SeedExt_SubSystemBase.generated.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(ClassGroup=(SeedExtension))
class SEEDEXT_CORE_API USeedExt_EngineSubSystem : public UEngineSubsystem, public ISeedExt_SubSystemInterface
{
	friend class FSeedExt_SubSystemCollector;

	GENERATED_BODY()

	//--------------------------------------------------//
public:
	template<class TSubSystem>
	FORCEINLINE static TSubSystem* _GetSubSystem(UClass* pInSubsystemClass=TSubSystem::StaticClass())
	{
		return Cast<TSubSystem>(GEngine->GetEngineSubsystemBase(pInSubsystemClass));
	}
	template<class TSubSystem>
	FORCEINLINE static bool _HasSubSystem(UClass* pInSubSystemClass=TSubSystem::StaticClass())
	{
		return (_GetSubSystem<TSubSystem>(pInSubSystemClass) != nullptr);
	}


protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override { Init(); }
	virtual void Deinitialize() override { Reset(); }

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override
	{
		if(GetClass() == USeedExt_EngineSubSystem::StaticClass())
			return false;

		return Super::ShouldCreateSubsystem(Outer);
	}

public:
	virtual UWorld* GetSubSystemWorld() override;
	virtual UClass* GetSubSystemClass() override;
	virtual FString GetSubSystemName() override;
	virtual ESeedExt_SubSystemType GetSubSystemType() const override { return ESeedExt_SubSystemType::Engine; }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(ClassGroup=(SeedExtension))
class SEEDEXT_CORE_API USeedExt_GameInstanceSubSystem : public UGameInstanceSubsystem, public ISeedExt_SubSystemInterface
{
	friend class FSeedExt_SubSystemCollector;

	GENERATED_BODY()

	//--------------------------------------------------//
public:
	template<class TSubSystem>
	FORCEINLINE static TSubSystem* _GetSubsystem(UObject* InWorldContextObject, UClass* InSubsystemClass=TSubSystem::StaticClass())
	{
		UWorld* CurrentWorld = (InWorldContextObject!=nullptr) ? GEngine->GetWorldFromContextObject(InWorldContextObject,EGetWorldErrorMode::LogAndReturnNull) : nullptr;
		if(CurrentWorld==nullptr || CurrentWorld->GetGameInstance()==nullptr)
			return nullptr;

		return Cast<TSubSystem>(CurrentWorld->GetGameInstance()->GetSubsystemBase(InSubsystemClass));
	}
	template<class TSubSystem>
	FORCEINLINE static bool _HasSubsystem(UObject* InWorldContextObject, UClass* InSubsystemClass=TSubSystem::StaticClass())
	{
		return (_GetSubsystem<TSubSystem>(InWorldContextObject, InSubsystemClass) != nullptr);
	}

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override { Init(); }
	virtual void Deinitialize() override { Reset(); }

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override
	{
		if(GetClass() == USeedExt_GameInstanceSubSystem::StaticClass())
			return false;

		return Super::ShouldCreateSubsystem(Outer);
	}

public:
	virtual UWorld* GetSubSystemWorld() override;
	virtual UClass* GetSubSystemClass() override;
	virtual FString GetSubSystemName() override;
	virtual ESeedExt_SubSystemType GetSubSystemType() const override { return ESeedExt_SubSystemType::GameInstance; }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////