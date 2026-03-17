// Copyright 2024. whitelilac, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ProjectKR/Landscape/ProjectKR_LandscapeManager.h"

#include "ProjectKR_LandscapeGenerator.generated.h"


USTRUCT(BlueprintType)
struct PROJECTKREDITOR_API FProjectKR_BiomeEnvironment
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR")
	FName BiomeName = NAME_None;

	// 기후 타겟
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR",meta=(ClampMin=0,ClampMax=1))
	float Temperature = 0.5f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR",meta=(ClampMin=0,ClampMax=1))
	float Humidity = 0.5f;

	// 고도 제한
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR")
	float MinHeight = 0.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR")
	float MaxHeight = 1.0f;

	// 지형 특성
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR")
	float HeightOffset = 0.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR")
	float HeightMultiplier = 1.0f;
};

USTRUCT(BlueprintType)
struct PROJECTKREDITOR_API FProjectKR_BiomeInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR")
	FProjectKR_BiomeEnvironment BiomeInfo;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR")
	TSoftObjectPtr<class ULandscapeLayerInfoObject> BiomeInfoObject; 
};

UCLASS(ClassGroup="KR", Blueprintable)
class PROJECTKREDITOR_API AProjectKR_LandscapeGenerator : public AActor
{
	GENERATED_BODY()

public:
	AProjectKR_LandscapeGenerator();

	//~ Begin UObject Interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UObject Interface
	
protected:
	/** Main function to generate the landscape. Accessible via a button in the Details panel. */
	UFUNCTION(CallInEditor,Category="Landscape Generate",DisplayName="Generate Landscape",meta=(DisplayPriority="0"))
	void TryToGenerateLandscape();
	/** Clears the currently managed landscape, if any. */
	UFUNCTION(CallInEditor,Category="Landscape Generate",DisplayName="Clear Landscape",meta=(DisplayPriority="1"))
	void ClearManagedLandscape();
	UFUNCTION(CallInEditor,Category="Landscape Generate",DisplayName="Found Variables",meta=(DisplayPriority="2"))
	void TryToFindVariables();

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

	/** Overall scale of the noise pattern. Larger values zoom out, creating larger features. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "KR|Landscape Settings")
	float MicroNoiseScale = 0.005f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "KR|Landscape Settings")
	float ClimateNoiseScale = 0.0001f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "KR|Landscape Settings")
	float Persistence = 0.5f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "KR|Landscape Settings")
	float Lacunarity = 2.0f;

	/** Number of noise layers to stack. More octaves add more detail. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "KR|Landscape Settings",meta=(ClampMin="1",ClampMax="8"))
	int32 Octaves = 6;
	
	/** Number of sections per component. */
	UPROPERTY(EditAnywhere, Category = "Landscape Settings|Resolution")
	int32 SectionsPerComponent = 1;

	/** The material to apply to the generated landscape. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KR|Landscape Settings")
	TObjectPtr<class UMaterialInterface> LandscapeMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="KR|Landscape Settings")
	TObjectPtr<class URuntimeVirtualTexture> RVT_HeightMap = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="KR|Landscape Settings")
	TObjectPtr<class URuntimeVirtualTexture> RVT_BiomeMap = nullptr;
	/** Default Landscape Layer Info Object to use when no biomes are defined. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KR|Landscape Settings")
	TMap<EProjectKR_LandscapeBiomeType, TObjectPtr<class ULandscapeLayerInfoObject>> LandscapeBiomeInfoObject_List;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KR|Landscape Settings")
	TObjectPtr<class ULandscapeLayerInfoObject> DefaultLayerInfoObject = nullptr;
	UPROPERTY(EditAnywhere, Category = "KR|Landscape Settings")
	TMap<EProjectKR_LandscapeBiomeType, FProjectKR_BiomeInfo> LandscapeBiomeInfo_List;
	UPROPERTY(EditAnywhere, Category = "KR|Landscape Settings")
	TMap<EProjectKR_LandscapeBiomeType, TSoftObjectPtr<UMaterialInterface>> LandscapeBiomeMaterial_List;
	

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
};