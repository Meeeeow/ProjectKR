// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ESeedExt_LoadingStateType : uint8
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
enum class ESeedExt_PlayingStateType : uint8
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
enum class ESeedExt_ClearReason : uint8
{
	None = 0		UMETA(Hidden),
	Exit			UMETA(DisplayName="Exit"),
	Shutdown		UMETA(DisplayName="Shutdown"),
	LoadMap			UMETA(DisplayName="LoadMap"),
	Num,
};