// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/TopLevelAssetPath.h"

PROJECTKR_API DECLARE_LOG_CATEGORY_EXTERN(ProjectKR_Log, Log, All);
PROJECTKR_API DECLARE_LOG_CATEGORY_EXTERN(ProjectKR_Warning, Warning, All);
PROJECTKR_API DECLARE_LOG_CATEGORY_EXTERN(ProjectKR_Error, Error, All);
PROJECTKR_API DECLARE_LOG_CATEGORY_EXTERN(ProjectKR_Assert, Error, All);

PROJECTKR_API void ProjectKR_MessageBox(const TCHAR* sInTitle, const TCHAR* sInFormat, ...);
PROJECTKR_API EAppReturnType::Type ProjectKR_MessageBox(EAppMsgType::Type eInMsgType, const TCHAR* sInTitle, const TCHAR* sInFormat, ...);

#define PROJECTKR_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::Printf(TEXT("File:%s,Line:%d"),TEXT(__FILE__),__LINE__) + TEXT(")"))
#define PROJECTKR_NONESTRING FString()
#define PROJECTKR_NONETEXT FText()
#define PROJECTKR_NONEPATH FTopLevelAssetPath()

UENUM(BlueprintType)
enum class EProjectKR_ClearReason : uint8
{
	None = 0		UMETA(Hidden),
	Exit			UMETA(DisplayName="Exit"),
	Shutdown		UMETA(DisplayName="Shutdown"),
	LoadMap			UMETA(DisplayName="LoadMap"),
	Num,
};

UENUM(BlueprintType)
enum class EProjectKR_PlayingStateType : uint8
{
	None = 0,
	NotReady,
	Ready,
	Play,
	Scaled,
	Pause,
	Stop,
	Num,
};

UENUM(BlueprintType)
enum class EProjectKR_LoadingStateType : uint8
{
	None = 0,
	NotLoading,
	Loading,
	LoadComplete,
	UnLoading,
	UnLoadComplete,
	Num,
};

UENUM(BlueprintType)
enum class EProjectKR_VisibleLayerType : uint8
{
	None = 0,
	World,
	Cinematic,
	UI,
	System,
	Num,
};

UENUM(BlueprintType)
enum class EProjectKR_SubSystemTickType : uint8
{
	None = 0,
	System = 1		UMETA(DisplayName="System"),
	World = 1 << 1	UMETA(DisplayName="World"),
	Scaled = 1 << 2	UMETA(DisplayName="Scaled"),

	SystemAndWorld = System | World,
	ScaledWorld = World | Scaled,

	Num,
};
ENUM_CLASS_FLAGS(EProjectKR_SubSystemTickType)

UENUM(BlueprintType)
enum class EProjectKR_SubSystemType : uint8
{
	None = 0			UMETA(Hidden),
	Engine				UMETA(DisplayName="EngineSubSystem"),
	Editor				UMETA(DisplayName="EditorSubSYstem"),
	GameInstance		UMETA(DisplayName="GameInstanceSubSystem"),
	World				UMETA(DisplayName="WorldSubSystem"),
	LocalPlayer			UMETA(DisplayName="LocalPlayerSubSystem"),
	Num,
};

namespace ProjectKR_Define
{
	
}