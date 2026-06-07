// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectKR_InputReceiveInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UProjectKR_InputReceiveInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTKR_API IProjectKR_InputReceiveInterface
{
	GENERATED_BODY()

public:
	virtual void HandleMoveInput(const FVector2D& InAxis) = 0;
	virtual void HandleLookInput(const FVector2D& InAxis) = 0;
	virtual void HandleJumpStart() = 0;
	virtual void HandleJumpStop() = 0;
};
