// Copyright 2024. whitelilac, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SeedExt_Core/Interface/SeedExt_DebugPropertyInterface.h"

#include "ProjectKR/Landscape/ProjectKR_LandscapeManager.h"

#include "ProjectKR_LandscapeGenerator.generated.h"

UCLASS(ClassGroup="KR", Blueprintable)
class PROJECTKREDITOR_API AProjectKR_LandscapeGenerator : public AActor, public ISeedExt_DebugPropertyInterface
{
	GENERATED_BODY()

public:
	AProjectKR_LandscapeGenerator();

#if WITH_EDITOR
	//~ Begin UObject Interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UObject Interface
#endif

	FString GetTextureFilePath() const;
	
protected:
	/** Main function to generate the landscape. Accessible via a button in the Details panel. */
	UFUNCTION(CallInEditor,Category="Step2 Generate Landscape",DisplayName="Generate Landscape",meta=(DisplayPriority="0"))
	void TryToGenerateLandscape();
	void CalculateBiomeWeights(const float InTemperature, const float InHumidity, const float InNormalizedHeight, TMap<FName, float>& OutWeightsMap) const;
	
private:
	static constexpr int32 MaxBiomeNum = 16;
	struct FBiomeWeightInfo
	{
		FName BiomeName = NAME_None;
		EProjectKR_LandscapeBiomeType BiomeType = EProjectKR_LandscapeBiomeType::None;
		float BiomeWeight = 0.f;
	};
	int32 CalculateBiomeWeightStack(float InTemperature, float InHumidity, float InNormalizedHeight, AProjectKR_LandscapeGenerator::FBiomeWeightInfo* OutBuffer, int32 InBufferSize) const;
	
protected:
	/** Clears the currently managed landscape, if any. */
	UFUNCTION(CallInEditor,Category="Step2 Generate Landscape",DisplayName="Clear Landscape",meta=(DisplayPriority="1"))
	void ClearManagedLandscape();
	UFUNCTION(CallInEditor,Category="Step2 Generate Landscape",DisplayName="Found Variables",meta=(DisplayPriority="2"))
	void TryToFindVariables();
	UFUNCTION(CallInEditor,Category="Step2 Generate Landscape",DisplayName="Change Landscape GridSize",meta=(DisplayPriority="3"))
	void TryToChangeLandscapeGridSize();

	UFUNCTION(CallInEditor,Category="Step1 Generate Texture",DisplayName="Bake Height Texture",meta=(DisplayPriority="0"))
	void BakeHeightMap();
	UFUNCTION(CallInEditor,Category="Step1 Generate Texture",DisplayName="Bake Environment Texture",meta=(DisplayPriority="1"))
	void BakeEnvironmentMap();
	UFUNCTION(CallInEditor,Category="Step3 Generate Foliage",DisplayName="Spawn Foliage",meta=(DisplayPriority="1"))
	void SpawnBiomeFoliage();
	UFUNCTION(CallInEditor,Category="Step3 Generate Foliage",DisplayName="Despawn Foliage",meta=(DisplayPriority="2"))
	void DespawnBiomeFoliage();

	UTexture2D* SaveArrayToTexture(const FString& InAssetName, int32 InSizeX, int32 InSizeY, const TArray<FColor>& InPixel_List);
	bool LoadArrayFromTexture(UTexture2D* InTexture, TArray<FColor>& OutPixel_List) const;

	/** Number of components in the X direction. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Landscape Settings", meta=(ClampMin="1",UIMin="1"))
	int32 ComponentCountX = 8;
	/** Number of components in the Y direction. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Landscape Settings", meta=(ClampMin="1",UIMin="1"))
	int32 ComponentCountY = 8;
	/** Number of World Partition Grid Size. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Landscape Settings", meta=(ClampMin="1",UIMin="1"))
	int32 WorldPartitionGridSize = 2;
	/** Number of quads per section. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Landscape Settings")
	int32 QuadsPerSection = 63;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Landscape Settings", meta=(ClampMin="-1500"))
	int32 BaseHeight = 35575;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Landscape Settings")
	int32 HeightAmplitude = 15000;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Landscape Settings", meta=(ClampMin="0.1", UIMin="0.1"))
	float RedistributionFactor = 2.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Landscape Settings")
	float MacroNoiseScale = 2000.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Landscape Settings", meta=(ClampMin="0.0",UIMin="0.0",ClampMax="1.0",UIMax="1.0"))
	float MacroNoiseInfluence = 0.5f;

	/** Seed for the random number generator to ensure reproducibility. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Landscape Settings")
	int32 Seed = 1;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Landscape Settings")
	int32 FoliageSeed = 32;

	/** Overall scale of the noise pattern. Larger values zoom out, creating larger features. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "KR|Landscape Settings")
	float MicroNoiseScale = 0.005f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "KR|Landscape Settings")
	float ClimateNoiseScale = 0.0001f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "KR|Landscape Settings")
	float Persistence = 0.5f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "KR|Landscape Settings")
	float Lacunarity = 2.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "KR|Landscape Settings")
	int32 HeightMapSmoothPassNum = 2; 
	
	/** Number of noise layers to stack. More octaves add more detail. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "KR|Landscape Settings",meta=(ClampMin="1",ClampMax="8"))
	int32 Octaves = 6;
	
	/** Number of sections per component. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "KR|Landscape Settings")
	int32 SectionsPerComponent = 1;

	/** The material to apply to the generated landscape. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KR|Landscape Settings")
	TObjectPtr<class UMaterialInterface> LandscapeMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="KR|Landscape Settings")
	TObjectPtr<class URuntimeVirtualTexture> RVT_HeightMap = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="KR|Landscape Settings")
	TObjectPtr<class URuntimeVirtualTexture> RVT_BiomeMap = nullptr;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Landscape Settings")
	TObjectPtr<class ASeedExt_Sun> SunActor = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Landscape Texture")
	TObjectPtr<class UTexture2D> BakedHeightMap = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Landscape Texture")
	TObjectPtr<class UTexture2D> BakedEnvironmentMap = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "KR|Landscape Settings")
	TMap<EProjectKR_LandscapeBiomeType, FProjectKR_BiomeInfo> LandscapeBiomeInfo_List;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Landscape Settings")
	FName LandscapeName = FName(TEXT("KR_GenerateLandscape"));
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Landscape Settings")
	FName HeightVolumeName = FName(TEXT("KR_GenerateHeightVolume"));
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Landscape Settings")
	FName BiomeVolumeName = FName(TEXT("KR_GenerateBiomeVolume"));

private:
	/** The ALandscape actor currently managed by this generator. */
	UPROPERTY(Transient)
	TWeakObjectPtr<class ALandscape> ManagedLandscape = nullptr;
	UPROPERTY(Transient)
	TWeakObjectPtr<class ARuntimeVirtualTextureVolume> ManagedHeightVolume = nullptr;
	UPROPERTY(Transient)
	TWeakObjectPtr<class ARuntimeVirtualTextureVolume> ManagedBiomeVolume = nullptr;

	FVector2D PixelToWorld(const int32 InPixelX, const int32 InPixelY) const;

	void BroadcastNotification(const FString& InNotificationString) const;

	void SmoothHeightMap(TArray<FColor>& InOutPixel_List, int32 InSizeX, int32 InSizeY) const;
	
public:
	static EProjectKR_LandscapeBiomeType FindDominantBiome(const TMap<EProjectKR_LandscapeBiomeType, float>& InWeightMap);
};