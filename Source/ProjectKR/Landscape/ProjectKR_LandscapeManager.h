// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SeedExt_Core/SubSystem/SeedExt_SubSystemBase.h"

#include "ProjectKR_LandscapeManager.generated.h"

UENUM(BlueprintType)
enum class EProjectKR_LandscapeBiomeType : uint8
{
	None = 0,
	Dirt,
	GrassA,
	Rock,
	SandA,
	SandB,
	Snow,
	Num			UMETA(Hidden),
};

UCLASS()
class PROJECTKR_API UProjectKR_LandscapeManager : public USeedExt_GameInstanceSubSystem
{
	GENERATED_BODY()
};
