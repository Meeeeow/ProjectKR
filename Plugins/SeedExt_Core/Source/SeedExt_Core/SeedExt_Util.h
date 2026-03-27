// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SeedExt_Util.generated.h"

UCLASS()
class SEEDEXT_CORE_API USeedExt_Util : public UObject
{
	GENERATED_BODY()
};


static bool SeedExt_IsGameWorld(EWorldType::Type InWorldType)
{
	if(InWorldType==EWorldType::PIE || InWorldType==EWorldType::Game)
		return true;

	return false;
}
static bool SeedExt_IsGameWorld(UWorld* InWorld, bool InCheckTearingDown)
{
	if(InWorld == nullptr)
		return false;

	if(InCheckTearingDown==true && InWorld->bIsTearingDown==true)
		return false;

	return SeedExt_IsGameWorld(InWorld->WorldType);
}

template <typename T>
static T* SeedExt_SpawnActor(UObject* pInWorldContextObject, UClass* pInClass, const FVector& InLocation, const FRotator& InRotation, const FVector& InScale, const FString& InLabelName=FString(), bool bInNeedRootComponent=false)
{
	UWorld* pCurrentWorld = GEngine->GetWorldFromContextObject(pInWorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	check(pCurrentWorld != nullptr);

	const FTransform SpawnTransform = FTransform(InRotation,InLocation,InScale);
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* pNewActor = pCurrentWorld->SpawnActor(pInClass, &SpawnTransform, SpawnInfo);
	check(pNewActor != nullptr);

#if WITH_EDITOR
	if(InLabelName.IsEmpty() == false)
		pNewActor->SetActorLabel(InLabelName);
#endif

	if( (bInNeedRootComponent==true) && (pNewActor->GetRootComponent()==nullptr) ){
		USceneComponent* pRootComponent = NewObject<USceneComponent>(pNewActor, USceneComponent::GetDefaultSceneRootVariableName(), RF_Transactional);
		pRootComponent->Mobility = EComponentMobility::Movable;
#if WITH_EDITORONLY_DATA
		pRootComponent->bVisualizeComponent = false;
#endif

		pNewActor->SetRootComponent(pRootComponent);
		pNewActor->AddInstanceComponent(pRootComponent);

		pRootComponent->RegisterComponent();
	}

	return Cast<T>(pNewActor);
}

template<typename T> T* SeedExt_HexStringToPointer(const FString& InHexString)
{
	FString HexString = InHexString;
	if(HexString.Contains(TEXT("0x")) == true)
	{
		HexString.RemoveFromStart(TEXT("0x"));
	}
	
	check(HexString.Len() <= 16);

#if UE_BUILD_DEBUG
	const int32 CharCount = HexString.Len();
	const TCHAR* CharPos = *HexString;

	uint64 iOutValue = 0;
	uint8* pByte = ((uint8*)&iOutValue)+7;
	for(int32 Index=0; Index<16-CharCount; ++Index){	//생략된 0 처리.
		if((Index%2) == 1){
			*pByte = 0;
			pByte--;
		}
	}
	if((CharCount%2) == 1){	//홀수 처리.
		*pByte = TCharToNibble(*CharPos++);
		pByte--;
	}
	while( *CharPos ){
		*pByte = TCharToNibble(*CharPos++) << 4;
		*pByte += TCharToNibble(*CharPos++);
		pByte--;
	}
	return reinterpret_cast<T*>(iOutValue);
#else
	return nullptr;
#endif
}