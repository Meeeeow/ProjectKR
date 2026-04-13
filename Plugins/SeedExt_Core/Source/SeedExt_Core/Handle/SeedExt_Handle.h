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

#define SEEDEXT_REGIST_HANDLE_H_BODY(HandleClassPrefix,HandleClassName,InstanceClassPrefix,InstanceClassName)																			\
private:																																												\
	SEEDEXT_DECLARE_HANDLE_H_HANDLE_COUNT(HandleClassPrefix,HandleClassName)																											\
	friend class SEEDEXT_MACROSTRING(InstanceClassPrefix,InstanceClassName);																											\
																																														\
	static SEEDEXT_MACROSTRING(HandleClassPrefix,HandleClassName) _CreateHandle(SEEDEXT_MACROSTRING(InstanceClassPrefix,InstanceClassName)* InInstance, uint64 InHandleCount = 0)		\
	{																																													\
		SEEDEXT_MACROSTRING(HandleClassPrefix,HandleClassName) _NewHandle;																												\
		_NewHandle.SetHandleIndex( (InHandleCount == 0) ? _GetHandleCount() : InHandleCount );																							\
		return _NewHandle;																																								\
	}																																													\
																																														\
	static void _Reset()																																								\
	{																																													\
		_ResetHandleCount();																																							\
	}																																													\
																																														\
public:																																													\
	static const SEEDEXT_MACROSTRING(HandleClassPrefix,HandleClassName) None;																											\
																																														\
	static const FString _ToString(const SEEDEXT_MACROSTRING(HandleClassPrefix,HandleClassName)& InHandle)																				\
	{																																													\
		return FString::Printf(TEXT("%s::%llu"), TEXT(#HandleClassName), InHandle.HandleIndex);																							\
	}																																													\
	static const SEEDEXT_MACROSTRING(HandleClassPrefix,HandleClassName) _FromString(const FString& InString)																			\
	{																																													\
		check(InString.IsEmpty() == false);																																				\
																																														\
		TArray<FString> HandleToken_List;																																				\
		InString.ParseIntoArray(HandleToken_List,TEXT("::"),true);																														\
																																														\
		if( (HandleToken_List.Num() != 2) || (HandleToken_List[0] != TEXT(#HandleClassName) || HandleToken_List[1].IsNumeric() == false) )												\
		{																																												\
			return SEEDEXT_MACROSTRING(HandleClassPrefix,HandleClassName)();																											\
		}																																												\
																																														\
		SEEDEXT_MACROSTRING(HandleClassPrefix,HandleClassName) _NewHandle;																												\
		uint64 _NewHandleIndex = FCString::Strtoui64(*HandleToken_List[0], nullptr, 10);																								\
		return _NewHandle;																																								\
	}																																													\
	static const uint64 _ToIndex(const SEEDEXT_MACROSTRING(HandleClassPrefix,HandleClassName)& InHandle)																				\
	{																																													\
		check(InHandle.GetHandleIndex());																																				\
																																														\
		return InHandle.GetHandleIndex();																																				\
	}																																													\
	static const SEEDEXT_MACROSTRING(HandleClassPrefix,HandleClassName) _FromIndex(const uint64 InIndex)																				\
	{																																													\
		SEEDEXT_MACROSTRING(HandleClassPrefix,HandleClassName) _NewHandle;																												\
		_NewHandle.SetHandleIndex(InIndex);																																				\
		return _NewHandle;																																								\
	}																																													\

#define SEEDEXT_REGIST_HANDLE_H_END(HandleClassPrefix,HandleClassName,InstanceClassPrefix,InstanceClassName)																			\
inline uint32 GetTypeHash(const SEEDEXT_MACROSTRING(HandleClassPrefix,HandleClassName) InHandle)																						\
{																																														\
	return GetTypeHash(InHandle.GetHandleIndex());																																		\
};																																														\
typedef TDefaultMapHashableKeyFuncs<SEEDEXT_MACROSTRING(HandleClassPrefix,HandleClassName),SEEDEXT_MACROSTRING(InstanceClassPrefix,InstanceClassName)*,false> SEEDEXT_MACROSTRING_TRIPLE(HandleClassPrefix,HandleClassName,MapKeyFuncs);

#define SEEDEXT_REGIST_HANDLE_H(ApiName,HandleClassPrefix,HandleClassName,InstanceClassPrefix,InstanceClassName)																		\
struct ApiName SEEDEXT_MACROSTRING(HandleClassPrefix,HandleClassName) : public FSeedExt_Handle																							\
{																																														\
	SEEDEXT_REGIST_HANDLE_H_BODY(HandleClassPrefix,HandleClassName,InstanceClassPrefix,InstanceClassName)																				\
};																																														\
SEEDEXT_REGIST_HANDLE_H_END(HandleClassPrefix,HandleClassName,InstanceClassPrefix,InstanceClassName)

#define SEEDEXT_REGIST_HANDLE_CPP(HandleClassPrefix,HandleClassName)																													\
SEEDEXT_DECLARE_HANDLE_CPP_HANDLE_COUNT(HandleClassPrefix,HandleClassName)																												\
const SEEDEXT_MACROSTRING(HandleClassPrefix,HandleClassName) SEEDEXT_MACROSTRING(HandleClassPrefix,HandleClassName)::None = SEEDEXT_MACROSTRING(HandleClassPrefix,HandleClassName)(); 