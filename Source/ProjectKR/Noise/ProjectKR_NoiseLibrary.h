// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ProjectKR_NoiseLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTKR_API UProjectKR_NoiseLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Fractal Noise Algorithm
	 */
	UFUNCTION(BlueprintCallable,Category="Noise_Terrain")
	static float GetTerrainHeight(float InX, float InY, float InScale, int32 InOctaves, int32 InSeed);
};
