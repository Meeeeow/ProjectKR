// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ESeedExt_SubSystemType : uint8
{
	None = 0			UMETA(Hidden),
	Engine				UMETA(DisplayName="EngineSubSystem"),
	Editor				UMETA(DisplayName="EditorSubSYstem"),
	GameInstance		UMETA(DisplayName="GameInstanceSubSystem"),
	World				UMETA(DisplayName="WorldSubSystem"),
	LocalPlayer			UMETA(DisplayName="LocalPlayerSubSystem"),
	Num,
};

UENUM(BlueprintType)
enum class ESeedExt_SubSystemTickType : uint8
{
	None = 0,
	System = 1		UMETA(DisplayName="System"),
	World = 1 << 1	UMETA(DisplayName="World"),
	Scaled = 1 << 2	UMETA(DisplayName="Scaled"),
	
	SystemAndWorld = System | World,
	ScaledWorld = World | Scaled,
	
	Num,
};

DECLARE_LOG_CATEGORY_EXTERN(SeedExt_Log, Log, All);