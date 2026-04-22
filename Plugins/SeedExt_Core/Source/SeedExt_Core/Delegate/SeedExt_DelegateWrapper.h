// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SeedExt_Core/SeedExt_Define.h"

template<typename TDelegate>
class FSeedExt_DelegateWrapper
{
protected:
	TDelegate OnDelegate;
	TMap<void*, FDelegateHandle> DelegateHandle_List;

public:
	void BindDelegate(void* InDelegateObject, typename TDelegate::FDelegate* InDelegate);
	void UnbindDelegate(void* InDelegateObject);
	bool IsBindDelegate(void* InDelegateObject) const;
};
template <typename TDelegate> void FSeedExt_DelegateWrapper<TDelegate>::BindDelegate(void* InDelegateObject, typename TDelegate::FDelegate* InDelegate)
{
	check(InDelegateObject != nullptr);
	check(InDelegate != nullptr);

	if(DelegateHandle_List.Contains(InDelegateObject) == false)
	{
		const FDelegateHandle& DelegateHandle = OnDelegate.Add(*InDelegate);
		DelegateHandle_List.Add(InDelegateObject, DelegateHandle);
	}
}

template <typename TDelegate> void FSeedExt_DelegateWrapper<TDelegate>::UnbindDelegate(void* InDelegateObject)
{
	check(InDelegateObject != nullptr);
	
	FDelegateHandle* RemoveDelegateHandle = DelegateHandle_List.Find(InDelegateObject);

	if(RemoveDelegateHandle != nullptr && RemoveDelegateHandle->IsValid() == true)
	{
		OnDelegate.Remove(*RemoveDelegateHandle);
		DelegateHandle_List.Remove(InDelegateObject);
	}
}

template <typename TDelegate> bool FSeedExt_DelegateWrapper<TDelegate>::IsBindDelegate(void* InDelegateObject) const
{
	check(InDelegateObject != nullptr);
	return DelegateHandle_List.Contains(InDelegateObject);
}
#define SEEDEXT_DECLARE_DELEGATE_WRAPPER(DelegateWrapperClass,TDelegate,DelegateIdentifier)																\
protected:																																				\
	DelegateWrapperClass SEEDEXT_MACROSTRING_TRIPLE(On,DelegateIdentifier,Delegator);																	\
																																						\
public:																																					\
	void SEEDEXT_MACROSTRING_TRIPLE(Bind,DelegateIdentifier,Delegate)(void* InDelegateObject, TDelegate::FDelegate* InDelegate)							\
	{																																					\
		SEEDEXT_MACROSTRING_TRIPLE(On,DelegateIdentifier,Delegator).BindDelegate(InDelegateObject, InDelegate);											\
	}																																					\
	void SEEDEXT_MACROSTRING_TRIPLE(Unbind,DelegateIdentifier,Delegate)(void* InDelegateObject)															\
	{																																					\
		SEEDEXT_MACROSTRING_TRIPLE(On,DelegateIdentifier,Delegator).UnbindDelegate(InDelegateObject);													\
	}																																					\
	bool SEEDEXT_MACROSTRING_TRIPLE(IsBind,DelegateIdentifier,Delegate)(void* InDelegateObject)															\
	{																																					\
		return SEEDEXT_MACROSTRING_TRIPLE(On,DelegateIdentifier,Delegator).IsBindDelegate(InDelegateObject);											\
	}