// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_LandscapeFunctionLibrary.h"

float UProjectKR_LandscapeFunctionLibrary::GetTerrainHeight(float InX, float InY, float InScale, int32 InOctaves, float InPersistence, float InLacunarity, int32 InSeed)
{
	float Total = 0.0f;
	float Frequency = InScale;
	float Amplitude = 1.0f;
	float MaxValue = 0.0f;


	for(int32 Index=0; Index<InOctaves; ++Index)
	{
		FVector2D NoisePosition = FVector2D(InX * Frequency + (InSeed * 1.12f), InY * Frequency + (InSeed * 0.89f));
		
		float NoiseValue = FMath::PerlinNoise2D(NoisePosition);
		Total += NoiseValue * Amplitude;

		MaxValue += Amplitude;
		Amplitude *= InPersistence;
		Frequency *= InLacunarity;
	}

	// Return -1.0f ~ 1.0f;
	return Total / MaxValue;
}

float UProjectKR_LandscapeFunctionLibrary::GetRidgedNoise(float InX, float InY, float InScale, int32 InOctaves, int32 InSeed)
{
	float Total = 0.0f;
	float Frequency = InScale;
	float Amplitude = 1.0f;
	float MaxValue = 0.0f;

	for(int32 Index=0; Index<InOctaves; ++Index)
	{
		FVector2D SamplePos = FVector2D(InX * Frequency + (InSeed * 1.57f), InY * Frequency + (InSeed * 2.11f));
		
		float NoiseValue = FMath::PerlinNoise2D(SamplePos);
		
		NoiseValue = 1.0f - FMath::Abs(NoiseValue);
		NoiseValue = NoiseValue * NoiseValue;
		
		Total += NoiseValue * Amplitude;
		MaxValue += Amplitude;
		Amplitude *= 0.5f;
		Frequency *= 2.0f;
	}

	// Return 0.0f ~ 1.0f
	return Total;
}

void UProjectKR_LandscapeFunctionLibrary::ApplyDomainWarp(float& InX, float& InY, float InStrength, int32 InSeed)
{
	float WarpX = FMath::PerlinNoise2D(FVector2D(InX * 0.0005f, InY * 0.0005f + InSeed));
	float WarpY = FMath::PerlinNoise2D(FVector2D(InX * 0.0005f + 31.4f, InY * 0.0005f + InSeed + 7));

	InX += WarpX * InStrength;
	InY += WarpY * InStrength;
}

float UProjectKR_LandscapeFunctionLibrary::GetTemperatureAt(float InX, float InY, int32 InSeed, float InScale)
{
	const float Noise = UProjectKR_LandscapeFunctionLibrary::GetTerrainHeight(InX, InY, InScale, 2, 0.5f, 2.0f, InSeed + 100);
	return FMath::Clamp((Noise + 1.0f) * 0.5f, 0.0f, 1.0f);
}

float UProjectKR_LandscapeFunctionLibrary::GetHumidityAt(float InX, float InY, int32 InSeed, float InScale)
{
	const float Noise = UProjectKR_LandscapeFunctionLibrary::GetTerrainHeight(InX, InY, InScale, 2, 0.5f, 2.0f, InSeed + 200);
	return FMath::Clamp((Noise + 1.0f) * 0.5f, 0.0f, 1.0f);
}
