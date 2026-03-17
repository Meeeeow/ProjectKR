#include "ProjectKR_LandscapeGenerator.h"

#include <map>

#include "Landscape.h"
#include "LandscapeImportHelper.h"
#include "LandscapeLayerInfoObject.h"
#include "Editor.h"
#include "EditorModeManager.h"
#include "EditorModes.h"
#include "EngineUtils.h"
#include "FileHelpers.h"
#include "LandscapeSubsystem.h"

#include "Components/RuntimeVirtualTextureComponent.h"

#include "Kismet/GameplayStatics.h"

#include "ProjectKR/Landscape/ProjectKR_LandscapeFunctionLibrary.h"

#include "UObject/UObjectIterator.h"
#include "WorldPartition/WorldPartition.h"

#include "SeedExt_Core/SubSystem/SeedExt_SubSystemDefine.h"

#include "VT/RuntimeVirtualTextureVolume.h"

AProjectKR_LandscapeGenerator::AProjectKR_LandscapeGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
	bIsEditorOnlyActor = true;

	TryToFindVariables();
}

void AProjectKR_LandscapeGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	TryToFindVariables();
}

void AProjectKR_LandscapeGenerator::TryToGenerateLandscape()
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}

	const int32 QuadsPerComponent = SectionsPerComponent * QuadsPerSection;
	const int32 SizeX = ComponentCountX * QuadsPerComponent + 1;
	const int32 SizeY = ComponentCountY * QuadsPerComponent + 1;

	TMap<FName, TArray<uint8>> LandscapeData_List;
	for(TMap<EProjectKR_LandscapeBiomeType, FProjectKR_BiomeInfo>::TConstIterator It(LandscapeBiomeInfo_List); It; ++It)
	{
		TArray<uint8>& MapValue = LandscapeData_List.Add(It.Value().BiomeInfo.BiomeName);
		MapValue.SetNumZeroed(SizeX * SizeY);
	}

	TArray<uint16> HeightData_List;
	HeightData_List.Init(0, SizeX*SizeY);
	TArray<FLandscapeImportLayerInfo> LandscapeLayerData_List;
	LandscapeLayerData_List.Reserve(LandscapeBiomeInfo_List.Num());
	{
		const int32 TemperatureSeed = Seed + 100;
		const int32 HumiditySeed = Seed + 200;
		const int32 RidgeSeed = Seed + 300;
		
		for(int32 Index_Height=0; Index_Height<SizeY; ++Index_Height)
		{
			for(int32 Index_Width=0; Index_Width<SizeX; ++Index_Width)
			{
				int32 Index = Index_Height * SizeX + Index_Width;
				
				const float BaseNoiseValue = UProjectKR_LandscapeFunctionLibrary::GetTerrainHeight(Index_Width, Index_Height, MicroNoiseScale, Octaves, Persistence, Lacunarity, Seed);
				const float DetailNoiseValue = UProjectKR_LandscapeFunctionLibrary::GetTerrainHeight(Index_Width, Index_Height, MacroNoiseScale, 3, Persistence, Lacunarity, Seed + 5);

				const float RidgeNoise = UProjectKR_LandscapeFunctionLibrary::GetRidgedNoise(Index_Width, Index_Height, MacroNoiseScale * 2.0f, 4, RidgeSeed);
				const float HeightMask = FMath::Clamp((BaseNoiseValue + 0.2f) * 1.5f, 0.0f, 1.0f);
				
				const float CombinedNoiseValue = FMath::Lerp(BaseNoiseValue, RidgeNoise, HeightMask * 0.5f) + (DetailNoiseValue * 0.1f);

				float NormalizedNoiseValue = (CombinedNoiseValue + 1.0f) * 0.5f;
				NormalizedNoiseValue = FMath::Pow(NormalizedNoiseValue, RedistributionFactor);

				const float HeightFloat = static_cast<float>(BaseHeight) + (NormalizedNoiseValue * static_cast<float>(HeightAmplitude) * 2.0f) - static_cast<float>(HeightAmplitude);
				
				const uint16 HeightValue = static_cast<uint16>(FMath::Clamp(HeightFloat, 0.0f, 65535.0f));
				
				HeightData_List[Index] = HeightValue;

				FName DefaultLayerName = FName("Dirt");
				
				const float TemperatureValue = UProjectKR_LandscapeFunctionLibrary::GetTerrainHeight(Index_Width, Index_Height, ClimateNoiseScale, 2, Persistence, Lacunarity, TemperatureSeed);
				const float HumidityValue = UProjectKR_LandscapeFunctionLibrary::GetTerrainHeight(Index_Width, Index_Height, ClimateNoiseScale, 2, Persistence, Lacunarity, HumiditySeed);

				float FinalTemperatureValue = (TemperatureValue * 1.5f + 1.0f) * 0.5f;
				FinalTemperatureValue = FMath::Clamp(FinalTemperatureValue - (NormalizedNoiseValue * 0.1f), 0.0f, 1.0f);

				float FinalHumidityValue = (HumidityValue * 1.5f + 1.0f) * 0.5f;
				FinalHumidityValue = FMath::Clamp(FinalHumidityValue, 0.0f, 1.0f);

				float MinDistance = 100.f;
				for(TMap<EProjectKR_LandscapeBiomeType, FProjectKR_BiomeInfo>::TConstIterator It(LandscapeBiomeInfo_List); It; ++It)
				{
					const FProjectKR_BiomeEnvironment& BiomeInfo = It->Value.BiomeInfo;
					if(const float Distance = FVector2D::Distance(FVector2D(FinalTemperatureValue, FinalHumidityValue), FVector2D(BiomeInfo.Temperature, BiomeInfo.Humidity)); Distance <  MinDistance)
					{
						MinDistance = Distance;
						DefaultLayerName = BiomeInfo.BiomeName;
					}
				}
				
				LandscapeData_List[DefaultLayerName][Index] = 255;
			}
		}
	}

	TMap<FGuid, TArray<uint16>> HeightData_Map;
	HeightData_Map.Add(FGuid(), MoveTemp(HeightData_List));

	for(TMap<EProjectKR_LandscapeBiomeType, FProjectKR_BiomeInfo>::TConstIterator It(LandscapeBiomeInfo_List); It; ++It)
	{
		FLandscapeImportLayerInfo LandscapeImportLayerInfo;
		ULandscapeLayerInfoObject* LandscapeImportLayerInfoObject = It->Value.BiomeInfoObject.Get();
		if(LandscapeImportLayerInfoObject == nullptr)
		{
			LandscapeImportLayerInfoObject = It->Value.BiomeInfoObject.LoadSynchronous(); 
		}
		if(LandscapeImportLayerInfoObject->LayerName == NAME_None)
		{
			LandscapeImportLayerInfoObject->Modify();
			LandscapeImportLayerInfoObject->LayerName = It->Value.BiomeInfo.BiomeName;
		}
		LandscapeImportLayerInfo.LayerInfo = LandscapeImportLayerInfoObject;
		LandscapeImportLayerInfo.LayerData = LandscapeData_List[It->Value.BiomeInfo.BiomeName];
		
		LandscapeLayerData_List.Add(LandscapeImportLayerInfo);
	}
	
	TMap<FGuid, TArray<FLandscapeImportLayerInfo>> MaterialLayerDataPerLayers;
	MaterialLayerDataPerLayers.Add(FGuid(), LandscapeLayerData_List);

	if(ManagedLandscape.IsValid() == false)
	{
		ManagedLandscape = World->SpawnActor<ALandscape>(ALandscape::StaticClass(), GetActorLocation(), GetActorRotation());
	}

	ManagedLandscape->SetActorScale3D(FVector(100.0f));
	ManagedLandscape->LandscapeMaterial = LandscapeMaterial;

	ManagedLandscape->Import(
		FGuid::NewGuid(),
		0, 0, SizeX-1, SizeY-1,
		SectionsPerComponent,
		QuadsPerComponent,
		HeightData_Map,
		nullptr,
		MaterialLayerDataPerLayers,
		ELandscapeImportAlphamapType::Additive,
		nullptr
	);

	ULandscapeInfo* LandscapeInfo = ManagedLandscape->GetLandscapeInfo();
	if(LandscapeInfo != nullptr)
	{
		if(ULandscapeSubsystem* LandscapeSubsystem = World->GetSubsystem<ULandscapeSubsystem>())
		{
			LandscapeSubsystem->ChangeGridSize(LandscapeInfo, FMath::Max(0, WorldPartitionGridSize));
		}
	}
	if(ManagedLandscape->GetActorLabel(false) != LandscapeName.ToString())
	{
		ManagedLandscape->SetActorLabel(LandscapeName.ToString());
	}
	
	if(ManagedHeightVolume.IsValid() == true && RVT_HeightMap != nullptr)
	{
		ManagedHeightVolume->VirtualTextureComponent->SetVirtualTexture(RVT_HeightMap);

		FBox SphereBound = ManagedLandscape->GetComponentsBoundingBox();
		FVector Center, Extents;
		SphereBound.GetCenterAndExtents(Center, Extents);
		ManagedHeightVolume->SetActorLocation(Center);
		ManagedHeightVolume->SetActorScale3D(Extents/50.0f);

		ManagedLandscape->RuntimeVirtualTextures.Add(RVT_HeightMap);
	}

	if(ManagedBiomeVolume.IsValid() == true && RVT_BiomeMap != nullptr)
	{
		ManagedBiomeVolume->VirtualTextureComponent->SetVirtualTexture(RVT_BiomeMap);

		FBox SphereBound = ManagedLandscape->GetComponentsBoundingBox();
		FVector Center, Extents;
		SphereBound.GetCenterAndExtents(Center, Extents);
		ManagedBiomeVolume->SetActorLocation(Center);
		ManagedBiomeVolume->SetActorScale3D(Extents/50.0f);

		ManagedLandscape->RuntimeVirtualTextures.Add(RVT_BiomeMap);
	}
}

void AProjectKR_LandscapeGenerator::ClearManagedLandscape()
{
	if(ManagedLandscape.IsValid() == true && ManagedLandscape->GetWorld() == GetWorld())
	{
		TArray<AActor*> FoundProxies;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALandscapeProxy::StaticClass(), FoundProxies);
		for(AActor* Proxy : FoundProxies)
		{
			Proxy->Destroy();
		}
		
		GEngine->ForceGarbageCollection(true);
		
		UE_LOG(LogTemp, Log, TEXT("LandscapeGenerator: Cleared managed landscape."));
	}
	else if(ManagedLandscape.IsValid() == false)
	{
		UE_LOG(LogTemp, Log, TEXT("LandscapeGenerator: No managed landscape to clear (ManagedLandscape is not valid)."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LandscapeGenerator: Managed landscape exists but is not in the current world. Skipping destruction."));
	}
}
void AProjectKR_LandscapeGenerator::TryToFindVariables()
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}

	for(TActorIterator<ALandscape> It(World); It; ++It)
	{
		if(*It->GetActorLabel(false) == LandscapeName.ToString())
		{
			ManagedLandscape = *It;
			break;
		}
	}

	for(TActorIterator<ARuntimeVirtualTextureVolume> It(World); It; ++It)
	{
		if(*It->GetActorLabel(false) == HeightVolumeName.ToString())
		{
			ManagedHeightVolume = *It;
			break;
		}
	}

	if(ManagedHeightVolume == nullptr)
	{
		ManagedHeightVolume = World->SpawnActor<ARuntimeVirtualTextureVolume>(ARuntimeVirtualTextureVolume::StaticClass(), GetActorLocation(), GetActorRotation());
		ManagedHeightVolume->SetActorLabel(HeightVolumeName.ToString());
	}

	for(TActorIterator<ARuntimeVirtualTextureVolume> It(World); It; ++It)
	{
		if(*It->GetActorLabel(false) == BiomeVolumeName.ToString())
		{
			ManagedBiomeVolume = *It;
			break;
		}
	}

	if(ManagedBiomeVolume == nullptr)
	{
		ManagedBiomeVolume = World->SpawnActor<ARuntimeVirtualTextureVolume>(ARuntimeVirtualTextureVolume::StaticClass(), GetActorLocation(), GetActorRotation());
		ManagedBiomeVolume->SetActorLabel(BiomeVolumeName.ToString());
	}
}
