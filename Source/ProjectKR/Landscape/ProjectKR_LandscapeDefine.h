#pragma once

#include "CoreMinimal.h"

#include "FoliageType_InstancedStaticMesh.h"

#include "ProjectKR_LandscapeDefine.generated.h"

USTRUCT(BlueprintType)
struct PROJECTKR_API FProjectKR_BiomeFoliageEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR")
	TSoftObjectPtr<class UFoliageType_InstancedStaticMesh> FoliageInstancedStaticMesh;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR",meta=(ClampMin=0.001f,ToolTip="Unit is 100 Square meter."))
	float Density = 10.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR")
	FVector2D ScaleRange = FVector2D(0.8f, 1.2f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="KR", meta=(ClampMin=0.0f, ClampMax=90.0f))
	float MaxSlopeDegrees = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="KR", meta=(ClampMin=0.0f, ClampMax=1.0f))
	float MinNormalizedHeight = 0.05f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="KR", meta=(ClampMin=0.0f, ClampMax=1.0f))
	float MaxNormalizedHeight = 0.85f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="KR", meta=(ClampMin=0.0f, ClampMax=1.0f))
	float MinBiomeWeight = 0.35f;
};

USTRUCT(BlueprintType)
struct PROJECTKR_API FProjectKR_BiomeEnvironment
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
struct PROJECTKR_API FProjectKR_BiomeInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR")
	FProjectKR_BiomeEnvironment BiomeEnvironment;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR")
	TSoftObjectPtr<class ULandscapeLayerInfoObject> BiomeInfoObject;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="KR")
	TArray<FProjectKR_BiomeFoliageEntry> FoliageEntry_List;
};

