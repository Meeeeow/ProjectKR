// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_AbilityInputConfig.h"

const UInputAction* UProjectKR_AbilityInputConfig::GetInputActionByTag(const FGameplayTag& InInputTag) const
{
	for(int32 Index=0; Index<AbilityInputAction_List.Num(); ++Index)
	{
		const FProjectKR_AbilityInputAction& AbilityInputAction = AbilityInputAction_List[Index];
		if(AbilityInputAction.InputTag==InInputTag && AbilityInputAction.InputAction!=nullptr)
			return AbilityInputAction.InputAction;
	}

	for(int32 Index=0; Index<NativeInputAction_List.Num(); ++Index)
	{
		const FProjectKR_AbilityInputAction& NativeInputAction = NativeInputAction_List[Index];
		if(NativeInputAction.InputTag==InInputTag && NativeInputAction.InputAction!=nullptr)
			return NativeInputAction.InputAction;
	}

	return nullptr;
}
