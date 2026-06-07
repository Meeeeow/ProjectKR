// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

/**
 * 
 */
class PROJECTKR_API FProjectKR_GameplayTags
{
public:
};

namespace ProjectKR_Tags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Jump);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Idle);
	
}
