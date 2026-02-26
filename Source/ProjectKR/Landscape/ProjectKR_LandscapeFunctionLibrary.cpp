// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_LandscapeFunctionLibrary.h"

float UProjectKR_LandscapeFunctionLibrary::GetTerrainHeight(float InX, float InY, float InScale, int32 InOctaves, int32 InSeed)
{
	float Total = 0.0f;
	float Frequency = InScale;
	float Amplitude = 1.0f;
	float MaxValue = 0.0f;

	for(int32 Index=0; Index<InOctaves; ++Index)
	{
		float NoiseValue = FMath::PerlinNoise2D(FVector2D((InX + InSeed) * Frequency, (InY + InSeed) * Frequency));
		Total += NoiseValue * Amplitude;

		MaxValue += Amplitude;
		Amplitude *= 0.5f;
		Frequency *= 0.2f;
	}

	return Total / MaxValue;
}
