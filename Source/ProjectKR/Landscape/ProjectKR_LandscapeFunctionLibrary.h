// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectKR_LandscapeManager.h"

#include "ProjectKR_LandscapeFunctionLibrary.generated.h"

UCLASS()
class PROJECTKR_API UProjectKR_LandscapeFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Fractal Noise Algorithm */
	UFUNCTION(BlueprintCallable,Category="Landscape")
	static float GetTerrainHeight(float InX, float InY, float InScale, int32 InOctaves, int32 InSeed);
	// UFUNCTION(BlueprintCallable,Category="Landscape")
	// static EProjectKR_LandscapeBiomeType GetLandscapeBiomeType(float InX, float InY, float InZ);
};
