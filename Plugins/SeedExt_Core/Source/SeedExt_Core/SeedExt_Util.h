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
