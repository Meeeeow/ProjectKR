// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SeedExt_Core/SeedExt_Define.h"

#include "UObject/Object.h"
#include "SeedExt_Handle.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct SEEDEXT_CORE_API FSeedExt_Handle
{
	GENERATED_USTRUCT_BODY()

protected:
	uint64 HandleIndex = 0;

	void SetHandleIndex(const uint64 InHandleIndex)
	{
		HandleIndex = InHandleIndex;
	}

public:
	uint64 GetHandleIndex() const
	{
		return HandleIndex;
	}

	bool IsValid() const
	{
		return HandleIndex != 0;
	}

	bool operator==(const FSeedExt_Handle& InOther) const
	{
		return HandleIndex == InOther.HandleIndex;
	}

	bool operator!=(const FSeedExt_Handle& InOther) const
	{
		return HandleIndex != InOther.HandleIndex;
	}
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SEEDEXT_DECLARE_HANDLE_H_HANDLE_COUNT(HandleClassPrefix,HandleClassName)																										\
static uint64 _HandleCount;																																								\
FORCEINLINE static uint64 _GetHandleCount() { return ++_HandleCount; }																													\
FORCEINLINE static void _ResetHandleCount() { _HandleCount = 0; }

#define SEEDEXT_DECLARE_HANDLE_CPP_HANDLE_COUNT(HandleClassPrefix,HandleClassName)																										\
uint64 SEEDEXT_MACROSTRING(HandleClassPrefix,HandleClassName)::_HandleCount = 0;

#define SEEDEXT_REGIST_HANDLE_H_BODY()