// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectKR_LightInterface.generated.h"

UINTERFACE()
class UProjectKR_LightInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTKR_API IProjectKR_LightInterface
{
	GENERATED_BODY()

public:
	virtual void UpdateLightVisual() = 0;
};
