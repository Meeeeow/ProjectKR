#include "ProjectKR_LandscapeGenerator.h"

#include "Landscape.h"
#include "LandscapeLayerInfoObject.h"
#include "Editor.h"
#include "EngineUtils.h"
#include "FileHelpers.h"
#include "InstancedFoliageActor.h"
#include "LandscapeSubsystem.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/RuntimeVirtualTextureComponent.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/UObjectIterator.h"
#include "WorldPartition/WorldPartition.h"
#include "VT/RuntimeVirtualTextureVolume.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "SeedExt_Core/World/SeedExt_Sun.h"

#include "ProjectKR/Landscape/ProjectKR_LandscapeFunctionLibrary.h"

AProjectKR_LandscapeGenerator::AProjectKR_LandscapeGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
	bIsEditorOnlyActor = true;

	TryToFindVariables();
}

#if WITH_EDITOR
void AProjectKR_LandscapeGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	TryToFindVariables();
	ExportProperty();
}
#endif

FString AProjectKR_LandscapeGenerator::GetTextureFilePath() const
{
	return TEXT("/Game/Landscape/BakedTexture/");
}

void AProjectKR_LandscapeGenerator::TryToGenerateLandscape()
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}

	if(BakedHeightMap == nullptr)
	{
		BroadcastNotification(TEXT("BakedHeightMap Texture가 없습니다."));
		return;
	}
	if(BakedEnvironmentMap == nullptr)
	{
		BroadcastNotification(TEXT("BakedEnvironmentMap Texture가 없습니다."));
		return;
	}

	const int32 SizeX = BakedHeightMap->GetSizeX();
	const int32 SizeY = BakedHeightMap->GetSizeY();
	const int32 PixelCount = SizeX * SizeY;

	if(BakedEnvironmentMap->GetSizeX() != SizeX || BakedEnvironmentMap->GetSizeY() != SizeY)
	{
		BroadcastNotification(FString::Printf(TEXT("BakedEnvironmentMap의 Texture 크기는 %d * %d 입니다. ( %d * %d )"), BakedEnvironmentMap->GetSizeX(), BakedEnvironmentMap->GetSizeY(), SizeX, SizeY));
		return;
	}

	const int32 QuadsPerComponent = SectionsPerComponent * QuadsPerSection;
	if(QuadsPerComponent <= 0)
	{
		BroadcastNotification(TEXT("[LandscapeGenerator] QuadsPerSection 또는 SectionsPerComponent가 0입니다."));
		return;
	}
	if((SizeX - 1) % QuadsPerComponent != 0 || (SizeY - 1) % QuadsPerComponent != 0)
	{
		
		BroadcastNotification(FString::Printf(TEXT("[LandscapeGenerator] 텍스처 크기(%dx%d)가 현재 QuadsPerSection(%d) × SectionsPerComponent(%d) = QuadsPerComponent(%d)와 맞지 않습니다."), SizeX, SizeY, QuadsPerSection, SectionsPerComponent, QuadsPerComponent));
		return;
	}

	const int32 DerivedComponentCountX = (SizeX - 1) / QuadsPerComponent;
	const int32 DerivedComponentCountY = (SizeY - 1) / QuadsPerComponent;

	TArray<FColor> HeightPixel_List;
	if(LoadArrayFromTexture(BakedHeightMap, HeightPixel_List) == false)
	{
		return;
	}

	TArray<FColor> EnvironmentPixel_List;
	if(LoadArrayFromTexture(BakedEnvironmentMap, EnvironmentPixel_List) == false)
	{
		return;
	}

	TArray<uint16> HeightData_List;
	HeightData_List.SetNumUninitialized(PixelCount);
	for(int32 Index=0; Index<PixelCount; Index++)
	{
		const float NormalizedHeight = static_cast<float>(HeightPixel_List[Index].R) / 255.f;
		const float HeightFloat = static_cast<float>(BaseHeight) + (NormalizedHeight * static_cast<float>(HeightAmplitude) * 2.f) - static_cast<float>(HeightAmplitude);
		HeightData_List[Index] = static_cast<uint16>(FMath::Clamp(HeightFloat, 0.f, 65535.f));
	}

	TMap<FName, TArray<uint8>> LandscapeData_List;
	for(TMap<EProjectKR_LandscapeBiomeType, FProjectKR_BiomeInfo>::TConstIterator It(LandscapeBiomeInfo_List); It; ++It)
	{
		TArray<uint8>& MapValue = LandscapeData_List.Add(It.Value().BiomeEnvironment.BiomeName);
		MapValue.SetNumZeroed(PixelCount);
	}

	ParallelFor(SizeY, [&](int32 Index_Y)
	{
		FBiomeWeightInfo WeightBuffer[MaxBiomeNum];
		for(int32 Index_X=0; Index_X<SizeX; ++Index_X)
		{
			const int32 Index = Index_Y * SizeX + Index_X;
			const float NormalizedHeight = static_cast<float>(HeightPixel_List[Index].R) / 255.f;
			const float Temperature = static_cast<float>(EnvironmentPixel_List[Index].R) / 255.f;
			const float Humidity = static_cast<float>(EnvironmentPixel_List[Index].G) / 255.f;

			const int32 WeightCount = CalculateBiomeWeightStack(Temperature, Humidity, NormalizedHeight, WeightBuffer, MaxBiomeNum);
			for(int32 WeightIndex=0; WeightIndex<WeightCount; ++WeightIndex)
			{
				if(TArray<uint8>* LayerData = LandscapeData_List.Find(WeightBuffer[WeightIndex].BiomeName))
				{
					(*LayerData)[Index] = static_cast<uint8>(WeightBuffer[WeightIndex].BiomeWeight * 255.f);
				}
			}
		}
	}, EParallelForFlags::Unbalanced);

	// TMap<EProjectKR_LandscapeBiomeType, float> BiomeCoverage_List;
	// for(const TPair<EProjectKR_LandscapeBiomeType, FProjectKR_BiomeInfo>& Pair : LandscapeBiomeInfo_List)
	// {
	// 	BiomeCoverage_List.Add(Pair.Key, 0.0f);
	// }
	// for(int32 Index_Y=0; Index_Y<SizeY; Index_Y++)
	// {
	// 	for(int32 Index_X=0; Index_X<SizeX; Index_X++)
	// 	{
	// 		const int32 Index = Index_Y * SizeX + Index_X;
	//
	// 		const float NormalizedHeight = static_cast<float>(HeightPixel_List[Index].R) / 255.f;
	// 		const float Temperature = static_cast<float>(EnvironmentPixel_List[Index].R) / 255.f;
	// 		const float Humidity = static_cast<float>(EnvironmentPixel_List[Index].G) / 255.f;
	//
	// 		TMap<FName, float> WeightsMap;
	// 		CalculateBiomeWeights(Temperature, Humidity, NormalizedHeight, WeightsMap);
	//
	// 		if(WeightsMap.IsEmpty() == false)
	// 		{
	// 			FName DominantName = NAME_None;
	// 			float DominantWeight = 0.0f;
	// 			for(const TPair<FName, float>& Pair : WeightsMap)
	// 			{
	// 				if(TArray<uint8>* LayerData = LandscapeData_List.Find(Pair.Key))
	// 				{
	// 					(*LayerData)[Index] = static_cast<uint8>(Pair.Value * 255.f);
	// 				}
	// 				if(Pair.Value > DominantWeight)
	// 				{
	// 					DominantWeight = Pair.Value;
	// 					DominantName = Pair.Key;
	// 				}
	// 			}
	// 		
	// 			for(TMap<EProjectKR_LandscapeBiomeType, FProjectKR_BiomeInfo>::TConstIterator It(LandscapeBiomeInfo_List); It; ++It)
	// 			{
	// 				if(It->Value.BiomeEnvironment.BiomeName == DominantName)
	// 				{
	// 					BiomeCoverage_List[It->Key] += 1.0f;
	// 					break;
	// 				}
	// 			}
	// 		}
	// 	}
	// }
	
	TMap<FGuid, TArray<uint16>> HeightData_Map;
	HeightData_Map.Add(FGuid(), MoveTemp(HeightData_List));
	
	TArray<FLandscapeImportLayerInfo> LandscapeLayerData_List;
	LandscapeLayerData_List.Reserve(LandscapeBiomeInfo_List.Num());

	for(TMap<EProjectKR_LandscapeBiomeType, FProjectKR_BiomeInfo>::TConstIterator It(LandscapeBiomeInfo_List); It; ++It)
	{
		ULandscapeLayerInfoObject* LayerInfo = It->Value.BiomeInfoObject.Get();
		if(LayerInfo == nullptr)
			LayerInfo = It->Value.BiomeInfoObject.LoadSynchronous();
		
		if(LayerInfo == nullptr)
			continue;

		if(LayerInfo->LayerName != It->Value.BiomeEnvironment.BiomeName)
		{
			LayerInfo->Modify();
			LayerInfo->LayerName = It->Value.BiomeEnvironment.BiomeName;
			LayerInfo->MarkPackageDirty();
		}

		FLandscapeImportLayerInfo ImportInfo;
		ImportInfo.LayerName = It->Value.BiomeEnvironment.BiomeName;
		ImportInfo.LayerInfo = LayerInfo;
		ImportInfo.LayerData = LandscapeData_List[It->Value.BiomeEnvironment.BiomeName];
		LandscapeLayerData_List.Add(ImportInfo);
	}

	TMap<FGuid, TArray<FLandscapeImportLayerInfo>> MaterialLayerDataPerLayers;
	MaterialLayerDataPerLayers.Add(FGuid(), LandscapeLayerData_List);

	if(ManagedLandscape.IsValid() == false)
	{
		ManagedLandscape = World->SpawnActor<ALandscape>(ALandscape::StaticClass(), GetActorLocation(), GetActorRotation());
	}

	ManagedLandscape->SetActorScale3D(FVector(100.0f));
	ManagedLandscape->LandscapeMaterial = LandscapeMaterial;

	const int32 HalfSizeX = (SizeX-1) / 2.f;
	const int32 HalfSizeY = (SizeY-1) / 2.f;
	
	ManagedLandscape->Import(
		FGuid::NewGuid(),
		-HalfSizeX, -HalfSizeY, HalfSizeX, HalfSizeY,
		SectionsPerComponent,
		QuadsPerSection,
		HeightData_Map,
		nullptr,
		MaterialLayerDataPerLayers,
		ELandscapeImportAlphamapType::Additive,
		nullptr
	);

	ULandscapeInfo::RecreateLandscapeInfo(World, false);
	if(ULandscapeInfo* LandscapeInfo = ManagedLandscape->GetLandscapeInfo())
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

	auto SetupRVTVolume = [&](TWeakObjectPtr<ARuntimeVirtualTextureVolume> InVolume, TObjectPtr<URuntimeVirtualTexture> InTexture)
	{
		if(InVolume.IsValid() == false || InTexture == nullptr)
			return;

		InVolume->VirtualTextureComponent->SetVirtualTexture(InTexture);

		FBox SphereBound = ManagedLandscape->GetComponentsBoundingBox();
		FVector Center, Extents;
		SphereBound.GetCenterAndExtents(Center, Extents);
		InVolume->SetActorLocation(Center);
		InVolume->SetActorScale3D(Extents/50.0f);

		ManagedLandscape->RuntimeVirtualTextures.AddUnique(InTexture);
	};

	SetupRVTVolume(ManagedHeightVolume, RVT_HeightMap);
	SetupRVTVolume(ManagedBiomeVolume, RVT_BiomeMap);
}
void AProjectKR_LandscapeGenerator::CalculateBiomeWeights(const float InTemperature, const float InHumidity, const float InNormalizedHeight, TMap<FName, float>& OutWeightsMap) const
{
	OutWeightsMap.Empty();
	constexpr float GaussianKernel = 2.0f;
	float TotalWeight = 0.0f;

	for(TMap<EProjectKR_LandscapeBiomeType, FProjectKR_BiomeInfo>::TConstIterator It(LandscapeBiomeInfo_List); It; ++It)
	{
		const FProjectKR_BiomeEnvironment& Environment = It->Value.BiomeEnvironment;
		
		const float HeightCenter = (Environment.MinHeight + Environment.MaxHeight) * 0.5f + Environment.HeightOffset;
		const float HeightHalfRange = (Environment.MaxHeight - Environment.MinHeight) * 0.5f;
        
		if(HeightHalfRange < KINDA_SMALL_NUMBER)
			continue;
		
		const float HeightSigma = HeightHalfRange * FMath::Max(Environment.HeightMultiplier, 0.1f);
		const float HeightNormal = (InNormalizedHeight - HeightCenter) / HeightSigma;

		const float TemperatureNormal = (InTemperature - Environment.Temperature) / FMath::Max(Environment.TemperatureRange, KINDA_SMALL_NUMBER);
		const float HumidityNormal = (InHumidity - Environment.Humidity) / FMath::Max(Environment.HumidityRange, KINDA_SMALL_NUMBER);

		const float NormalizedDistanceSqrt = (TemperatureNormal * TemperatureNormal) + (HumidityNormal * HumidityNormal) + (HeightNormal * HeightNormal);
		const float Weight = FMath::Exp(-NormalizedDistanceSqrt * GaussianKernel);
		
		if(Weight < KINDA_SMALL_NUMBER)
			continue;

		OutWeightsMap.Add(Environment.BiomeName, Weight);
		TotalWeight += Weight;
	}

	if(TotalWeight > KINDA_SMALL_NUMBER)
	{
		for(TPair<FName, float>& Pair : OutWeightsMap)
		{
			Pair.Value /= TotalWeight;
		}
	}
}

int32 AProjectKR_LandscapeGenerator::CalculateBiomeWeightStack(float InTemperature, float InHumidity, float InNormalizedHeight, AProjectKR_LandscapeGenerator::FBiomeWeightInfo* OutBuffer, int32 InBufferSize) const
{
	int32 Count = 0;
	float TotalWeight = 0.0f;
	constexpr float GaussianKernel = 2.0f;

	for(TMap<EProjectKR_LandscapeBiomeType, FProjectKR_BiomeInfo>::TConstIterator It(LandscapeBiomeInfo_List); It; ++It)
	{
		if(Count >= InBufferSize)
			break;

		const FProjectKR_BiomeEnvironment& Environment = It->Value.BiomeEnvironment;

		const float HeightHalfRange = (Environment.MaxHeight - Environment.MinHeight) * 0.5f;
		if(HeightHalfRange < KINDA_SMALL_NUMBER)
			continue;

		const float HeightCenter = (Environment.MinHeight + Environment.MaxHeight) * 0.5f + Environment.HeightOffset;
		const float HeightSigma = HeightHalfRange * FMath::Max(Environment.HeightMultiplier, 0.1f);
		const float HeightNormal = (InNormalizedHeight - HeightCenter) / HeightSigma;

		const float TemperatureNormal = (InTemperature - Environment.Temperature) / FMath::Max(Environment.TemperatureRange, KINDA_SMALL_NUMBER);
		const float HumidityNormal = (InHumidity - Environment.Humidity) / FMath::Max(Environment.HumidityRange, KINDA_SMALL_NUMBER);

		const float DistanceSqrt = (TemperatureNormal*TemperatureNormal) + (HumidityNormal*HumidityNormal) + (HeightNormal*HeightNormal);

		const float Weight = FMath::Exp(-DistanceSqrt * GaussianKernel);
		if(Weight < KINDA_SMALL_NUMBER)
			continue;

		OutBuffer[Count].BiomeName = Environment.BiomeName;
		OutBuffer[Count].BiomeType = It->Key;
		OutBuffer[Count].BiomeWeight = Weight;
		++Count;
		TotalWeight += Weight;
	}

	if(TotalWeight > KINDA_SMALL_NUMBER)
	{
		for(int32 Index=0; Index<Count; ++Index)
		{
			OutBuffer[Index].BiomeWeight /= TotalWeight;
		}
	}
	
	return Count;
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

	auto FindOrSpawnRVTVolume = [&](TWeakObjectPtr<ARuntimeVirtualTextureVolume>& InRVTVolume, const FName& InLabel)
	{
		for(TActorIterator<ARuntimeVirtualTextureVolume> It(World); It; ++It)
		{
			if(*It->GetActorLabel(false) == InLabel.ToString())
			{
				InRVTVolume = *It;
				return;
			}
		}
		
		if(InRVTVolume == nullptr)
		{
			InRVTVolume = World->SpawnActor<ARuntimeVirtualTextureVolume>(ARuntimeVirtualTextureVolume::StaticClass(), GetActorLocation(), GetActorRotation());
			InRVTVolume->SetActorLabel(InLabel.ToString());
		}
	};

	FindOrSpawnRVTVolume(ManagedHeightVolume, HeightVolumeName);
	FindOrSpawnRVTVolume(ManagedBiomeVolume, BiomeVolumeName);

	if(SunActor == nullptr)
	{
		for(TActorIterator<ASeedExt_Sun> It(World); It; ++It)
		{
			SunActor = *It;
			break;
		}
	}
}

void AProjectKR_LandscapeGenerator::TryToChangeLandscapeGridSize()
{
	UWorld* World = GetWorld();
	if(World == nullptr)
		return;

	if(ManagedLandscape.IsValid() == false)
		return;
	
	ULandscapeInfo::RecreateLandscapeInfo(World, false);

	if(ULandscapeInfo* LandscapeInfo = ManagedLandscape->GetLandscapeInfo())
	{
		
	}
}

void AProjectKR_LandscapeGenerator::BakeHeightMap()
{
	const int32 QuadsPerComponent = SectionsPerComponent * QuadsPerSection;
	const int32 SizeX = ComponentCountX * QuadsPerComponent + 1;
	const int32 SizeY = ComponentCountY * QuadsPerComponent + 1;
	const int32 PixelCount = SizeX * SizeY;

	TArray<FColor> Pixel_List;
	Pixel_List.SetNumZeroed(PixelCount);

	int32 RidgeSeed = Seed + 300;
	
	// for(int32 Index_Y=0; Index_Y<SizeY; ++Index_Y)
	// {
	// 	for(int32 Index_X=0; Index_X<SizeX; Index_X++)
	// 	{
	// 		const float BaseNoise = UProjectKR_LandscapeFunctionLibrary::GetTerrainHeight(Index_X, Index_Y, MicroNoiseScale, Octaves, Persistence, Lacunarity, Seed);
	// 		const float DetailNoise = UProjectKR_LandscapeFunctionLibrary::GetTerrainHeight(Index_X, Index_Y, MacroNoiseScale, 3, Persistence, Lacunarity, Seed + 5);
	// 		const float RidgeNoise = UProjectKR_LandscapeFunctionLibrary::GetRidgedNoise(Index_X, Index_Y, MacroNoiseScale * 2.0f, 4, RidgeSeed);
	//
	// 		const float HeightMask = FMath::Clamp((BaseNoise + 0.2f) * 1.5f, 0.0f, 1.0f);
	// 		
	// 		const float CombinedNoise = FMath::Lerp(BaseNoise, RidgeNoise, HeightMask * 0.5f) + (DetailNoise * 0.1f);
	//
	// 		const float NormalizedNoise = FMath::Pow((CombinedNoise + 1.0f)*0.5f, RedistributionFactor);
	//
	// 		const uint8 HeightByte = static_cast<uint8>(FMath::Clamp(NormalizedNoise*255.0f, 0.0f, 255.0f));
	// 		Pixel_List[Index_Y * SizeX + Index_X] = FColor(HeightByte, HeightByte, HeightByte, 255.f);
	// 	}
	// }

	ParallelFor(SizeY, [&](int32 Index_Y)
	{
		for(int32 Index_X=0; Index_X<SizeX; Index_X++)
		{
			const float BaseNoise = UProjectKR_LandscapeFunctionLibrary::GetTerrainHeight(Index_X, Index_Y, MicroNoiseScale, Octaves, Persistence, Lacunarity, Seed);
			const float DetailNoise = UProjectKR_LandscapeFunctionLibrary::GetTerrainHeight(Index_X, Index_Y, MacroNoiseScale, 3, Persistence, Lacunarity, Seed + 5);
			const float RidgeNoise = UProjectKR_LandscapeFunctionLibrary::GetRidgedNoise(Index_X, Index_Y, MacroNoiseScale * 2.0f, 4, RidgeSeed);

			const float HeightMask = FMath::Clamp((BaseNoise + 0.2f) * 1.5f, 0.0f, 1.0f);
			
			const float CombinedNoise = FMath::Lerp(BaseNoise, RidgeNoise, HeightMask * 0.5f) + (DetailNoise * 0.1f);

			const float NormalizedNoise = FMath::Pow((CombinedNoise + 1.0f)*0.5f, RedistributionFactor);

			const uint8 HeightByte = static_cast<uint8>(FMath::Clamp(NormalizedNoise*255.0f, 0.0f, 255.0f));
			Pixel_List[Index_Y * SizeX + Index_X] = FColor(HeightByte, HeightByte, HeightByte, 255.f);
		}
	});

	for(int32 SmoothIndex=0; SmoothIndex<HeightMapSmoothPassNum; ++SmoothIndex)
	{
		SmoothHeightMap(Pixel_List, SizeX, SizeY);
	}

	TArray<FAssetData> AssetData_List;
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
		AssetRegistry.GetAssetsByPath(FName(GetTextureFilePath()), AssetData_List, true);
	}

	int32 MaxIndex = -1;
	const FString Prefix = TEXT("Texture_BakedHeightMap_");
	for(const FAssetData& AssetData : AssetData_List)
	{
		FString AssetName = AssetData.AssetName.ToString();
		if(AssetName.StartsWith(Prefix) == false)
			continue;

		FString IndexPart = AssetName.RightChop(Prefix.Len());
		if(IndexPart.IsNumeric() == false)
			continue;

		if(const int32 Index = FCString::Atoi(*IndexPart); Index > MaxIndex)
			MaxIndex = Index;
	}

	BakedHeightMap = SaveArrayToTexture(FString::Printf(TEXT("%s%d"),*Prefix, ++MaxIndex), SizeX, SizeY, Pixel_List);
}
void AProjectKR_LandscapeGenerator::BakeEnvironmentMap()
{
	TArray<FColor> HeightPixel_List;

	if(LoadArrayFromTexture(BakedHeightMap, HeightPixel_List) == false)
	{
		return;
	}

	const int32 SizeX = BakedHeightMap->GetSizeX();
	const int32 SizeY = BakedHeightMap->GetSizeY();
	const int32 PixelCount = SizeX * SizeY;

	TArray<FColor> EnvironmentPixel_List;
	EnvironmentPixel_List.SetNumUninitialized(PixelCount);

	const bool bHasSun = (SunActor != nullptr);

	ParallelFor(SizeY, [&](int32 Index_Y)
	{
		FBiomeWeightInfo WeightBuffer[MaxBiomeNum];
		for(int32 Index_X=0; Index_X<SizeX; ++Index_X)
		{
			const int32 Index = Index_Y * SizeX + Index_X;
			
			const float NormalizedHeight = HeightPixel_List[Index].R / 255.f;
			
			float Temperature = UProjectKR_LandscapeFunctionLibrary::GetTemperatureAt(static_cast<float>(Index_X), static_cast<float>(Index_Y), Seed, ClimateNoiseScale);
			float Humidity = UProjectKR_LandscapeFunctionLibrary::GetHumidityAt(static_cast<float>(Index_X), static_cast<float>(Index_Y), Seed, ClimateNoiseScale);

			if(bHasSun == true)
			{
				const FVector2D WorldPosition2D = PixelToWorld(Index_X, Index_Y);
				const FSeedExt_InfluenceState InfluenceState = SunActor->GetBiomeInfluenceStateAtLocation(FVector(WorldPosition2D.X, WorldPosition2D.Y, 0.f));
				Temperature = InfluenceState.ApplyToTemperature(Temperature);
				Humidity = InfluenceState.ApplyToHumidity(Humidity);
			}

			const int32 WeightCount = CalculateBiomeWeightStack(Temperature, Humidity, NormalizedHeight, WeightBuffer, MaxBiomeNum);

			EProjectKR_LandscapeBiomeType DominantBiomeType = EProjectKR_LandscapeBiomeType::None;
			float MaxWeight = -1.f;
			for(int32 Index_Inner=0; Index_Inner<WeightCount; ++Index_Inner)
			{
				if(WeightBuffer[Index_Inner].BiomeWeight > MaxWeight)
				{
					MaxWeight = WeightBuffer[Index_Inner].BiomeWeight;
					DominantBiomeType = WeightBuffer[Index_Inner].BiomeType;
				}
			}

			EnvironmentPixel_List[Index] = FColor(
				static_cast<uint8>(FMath::Clamp(Temperature * 255.f, 0.0f, 255.f)),
				static_cast<uint8>(FMath::Clamp(Humidity * 255.f, 0.0f, 255.f)),
				static_cast<uint8>(DominantBiomeType),
				255.f);
		}
	}, EParallelForFlags::Unbalanced);
	
	// for(int32 Index_Y=0; Index_Y<SizeY; Index_Y++)
	// {
	// 	for(int32 Index_X=0; Index_X<SizeX; Index_X++)
	// 	{
	// 		const int32 Index = Index_Y * SizeX + Index_X;
	//
	// 		const float NormalizedHeight = HeightPixel_List[Index].R / 255.f;
	//
	// 		float Temperature = UProjectKR_LandscapeFunctionLibrary::GetTemperatureAt(static_cast<float>(Index_X), static_cast<float>(Index_Y), Seed, ClimateNoiseScale);
	// 		float Humidity = UProjectKR_LandscapeFunctionLibrary::GetHumidityAt(static_cast<float>(Index_X), static_cast<float>(Index_Y), Seed, ClimateNoiseScale);
	//
	// 		if(bHasSun == true)
	// 		{
	// 			const FVector2D WorldPosition2D = PixelToWorld(Index_X, Index_Y);
	// 			const FVector WorldPosition(WorldPosition2D.X, WorldPosition2D.Y, 0.0f);
	//
	// 			const FSeedExt_InfluenceState InfluenceState = SunActor->GetBiomeInfluenceStateAtLocation(WorldPosition);
	// 			Temperature = InfluenceState.ApplyToTemperature(Temperature);
	// 			Humidity = InfluenceState.ApplyToHumidity(Humidity);
	// 		}
	//
	// 		TMap<FName, float> WeightsMap;
	// 		CalculateBiomeWeights(Temperature, Humidity, NormalizedHeight, WeightsMap);
	//
	// 		EProjectKR_LandscapeBiomeType DominantBiomeType = EProjectKR_LandscapeBiomeType::None;
	// 		float MaxWeight = -1.0f;
	// 		for(TMap<EProjectKR_LandscapeBiomeType, FProjectKR_BiomeInfo>::TConstIterator It(LandscapeBiomeInfo_List); It; ++It)
	// 		{
	// 			const FName& BName = It->Value.BiomeEnvironment.BiomeName;
	// 			if(const float* Weight = WeightsMap.Find(BName); Weight != nullptr && *Weight > MaxWeight)
	// 			{
	// 				MaxWeight = *Weight;
	// 				DominantBiomeType = It->Key;
	// 			}
	// 		}
	//
	// 		EnvironmentPixel_List[Index] = FColor(
	// 			static_cast<uint8>(FMath::Clamp(Temperature * 255.f, 0.0f, 255.f)),
	// 			static_cast<uint8>(FMath::Clamp(Humidity * 255.f, 0.0f, 255.f)),
	// 			static_cast<uint8>(DominantBiomeType),
	// 			255.f);
	// 	}
	// }
	
	TArray<FAssetData> AssetData_List;
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
		AssetRegistry.GetAssetsByPath(FName(GetTextureFilePath()), AssetData_List, true);
	}
	
	int32 MaxIndex = -1;
	const FString Prefix = TEXT("Texture_BakedEnvironmentMap_");
	for(const FAssetData& AssetData : AssetData_List)
	{
		FString Name = AssetData.AssetName.ToString();
		if(Name.StartsWith(Prefix) == false)
			continue;
		FString Num = Name.RightChop(Prefix.Len());
		if(Num.IsNumeric() == false)
			continue;
		
		if(const int32 Idx = FCString::Atoi(*Num); Idx > MaxIndex)
			MaxIndex = Idx;
	}

	BakedEnvironmentMap = SaveArrayToTexture(FString::Printf(TEXT("%s%d"), *Prefix, ++MaxIndex), SizeX, SizeY, EnvironmentPixel_List);
}
void AProjectKR_LandscapeGenerator::SpawnBiomeFoliage()
{
	UWorld* World = GetWorld();
	if(World == nullptr)
		return;

	if(ManagedLandscape.IsValid() == false)
		return;

	if(BakedHeightMap == nullptr || BakedEnvironmentMap == nullptr)
		return;

	TArray<FColor> HeightPixel_List, EnvironmentPixel_List;
	if(LoadArrayFromTexture(BakedHeightMap, HeightPixel_List) == false)
		return;
	if(LoadArrayFromTexture(BakedEnvironmentMap, EnvironmentPixel_List) == false)
		return;

	const int32 SizeX = BakedHeightMap->GetSizeX();
	const int32 SizeY = BakedHeightMap->GetSizeY();
	if(HeightPixel_List.Num() != SizeX * SizeY)
		return;

	DespawnBiomeFoliage();

	AInstancedFoliageActor* InstancedFoliageActor = AInstancedFoliageActor::GetInstancedFoliageActorForCurrentLevel(World, true);
	if(InstancedFoliageActor == nullptr)
		return;

	FRandomStream RandomStream(FoliageSeed);

	const float HeightRangeCM = static_cast<float>(HeightAmplitude) * 2.0f/ 256.0f * 100.f;
	constexpr float PixelSpaceCM = 100.f;
	const float HeightScale = HeightRangeCM / (PixelSpaceCM * 255.0f);

	auto GetNormalHeight = [&](int32 X, int32 Y)
	{
		X = FMath::Clamp(X, 0, SizeX - 1);
		Y = FMath::Clamp(Y, 0, SizeY - 1);
		return static_cast<float>(HeightPixel_List[Y * SizeX + X].R) / 255.f;
	};

	for(TMap<EProjectKR_LandscapeBiomeType, FProjectKR_BiomeInfo>::TConstIterator It(LandscapeBiomeInfo_List); It; ++It)
	{
		const FProjectKR_BiomeInfo& BiomeInfo = It->Value;

		for(const FProjectKR_BiomeFoliageEntry& FoliageEntry : BiomeInfo.FoliageEntry_List)
		{
			if(FoliageEntry.FoliageInstancedStaticMesh.IsNull() == true)
				continue;

			UFoliageType_InstancedStaticMesh* FoliageMesh = FoliageEntry.FoliageInstancedStaticMesh.LoadSynchronous();
			if(FoliageMesh == nullptr)
				continue;

			const float PixelsPerInstance = 10000.0f / FMath::Max(FoliageEntry.Density, 0.001f);
			const float StepPixel = FMath::Sqrt(PixelsPerInstance);

			TArray<FFoliageInstance> FoliageInstance_List;
			FoliageInstance_List.Reserve(static_cast<int32>(static_cast<float>(SizeX * SizeY) / PixelsPerInstance * 1.2f));
			for(float SampleY=0.f; SampleY<static_cast<float>(SizeY); SampleY+=StepPixel)
			{
				for(float SampleX=0.f; SampleX<static_cast<float>(SizeX); SampleX+=StepPixel)
				{
					const float JitterX = RandomStream.FRandRange(-StepPixel * 0.5f, StepPixel * 0.5f);
					const float JitterY = RandomStream.FRandRange(-StepPixel * 0.5f, StepPixel * 0.5f);

					const float PixelX = FMath::Clamp(static_cast<int32>(SampleX + JitterX), 0, SizeX - 1);
					const float PixelY = FMath::Clamp(static_cast<int32>(SampleY + JitterY), 0, SizeY - 1);
					
					const int32 Index = PixelY * SizeX + PixelX;

					const float NormalizedHeight = GetNormalHeight(PixelX, PixelY);
					if(NormalizedHeight < FoliageEntry.MinNormalizedHeight || NormalizedHeight > FoliageEntry.MaxNormalizedHeight)
						continue;

					const float SlopeX = (GetNormalHeight(PixelX+1, PixelY) - GetNormalHeight(PixelX-1, PixelY)) * 0.5f;
					const float SlopeY = (GetNormalHeight(PixelX, PixelY+1) - GetNormalHeight(PixelX, PixelY-1)) * 0.5f;

					const float SlopeDegree = FMath::RadiansToDegrees(FMath::Atan(FMath::Sqrt(SlopeX*SlopeX + SlopeY*SlopeY) * HeightScale));
					if(SlopeDegree > FoliageEntry.MaxSlopeDegrees)
						continue;

					const float Temperature = static_cast<float>(EnvironmentPixel_List[Index].R) / 255.0f;
					const float Humidity = static_cast<float>(EnvironmentPixel_List[Index].G) / 255.0f;
					TMap<FName, float> WeightMap;
					CalculateBiomeWeights(Temperature, Humidity, NormalizedHeight, WeightMap);

					const FName& BiomeName = BiomeInfo.BiomeEnvironment.BiomeName;
					const float* BiomeWeight = WeightMap.Find(BiomeName);
					if(BiomeWeight == nullptr || *BiomeWeight < FoliageEntry.MinBiomeWeight)
						continue;

					const FVector2D WorldXY = PixelToWorld(PixelX, PixelY);
					FHitResult HitResult;
					if(World->LineTraceSingleByChannel(HitResult,FVector(WorldXY.X, WorldXY.Y,  500000.0f),FVector(WorldXY.X, WorldXY.Y, -500000.0f), ECC_WorldStatic) == false)
						continue;

					const float FoliageScale = RandomStream.FRandRange(FoliageEntry.ScaleRange.X, FoliageEntry.ScaleRange.Y);
					FFoliageInstance Inst;
					Inst.Location = HitResult.Location;
					Inst.Rotation = FRotator(0.0f, RandomStream.FRandRange(0.0f, 360.0f), 0.0f);
					Inst.DrawScale3D = FVector3f(FoliageScale);
					FoliageInstance_List.Add(Inst);
				}
			}

			if(FoliageInstance_List.Num() > 0)
			{
				FFoliageInfo* FoliageInfo = InstancedFoliageActor->FindOrAddMesh(FoliageEntry.FoliageInstancedStaticMesh.Get());
				if(FoliageInfo != nullptr)
				{
					TArray<const FFoliageInstance*> FoliageInstancePtr_List;
					FoliageInstancePtr_List.Reserve(FoliageInstance_List.Num());
					for(const FFoliageInstance& FoliageInstance : FoliageInstance_List)
					{
						FoliageInstancePtr_List.Add(&FoliageInstance);
					}
					FoliageInfo->AddInstances(FoliageEntry.FoliageInstancedStaticMesh.Get(), FoliageInstancePtr_List);
				}
			}
		}
	}
}
void AProjectKR_LandscapeGenerator::DespawnBiomeFoliage()
{
	UWorld* World = GetWorld();
	if(World == nullptr)
		return;

	AInstancedFoliageActor* InstancedFoliageActor = AInstancedFoliageActor::GetInstancedFoliageActorForCurrentLevel(World, false);
	if(InstancedFoliageActor == nullptr)
		return;

	for(TMap<EProjectKR_LandscapeBiomeType, FProjectKR_BiomeInfo>::TConstIterator It(LandscapeBiomeInfo_List); It; ++It)
	{
		for(const FProjectKR_BiomeFoliageEntry& FoliageEntry : It->Value.FoliageEntry_List)
		{
			if(FoliageEntry.FoliageInstancedStaticMesh.IsNull() == true)
				continue;
			
			UFoliageType_InstancedStaticMesh* FoliageInstancedMesh = FoliageEntry.FoliageInstancedStaticMesh.Get();
			if(FoliageInstancedMesh == nullptr)
				continue;

			if(FFoliageInfo* FoliageInfo = InstancedFoliageActor->FindInfo(FoliageInstancedMesh))
			{
				TArray<int32> Indices_List;
				Indices_List.Reserve(FoliageInfo->Instances.Num());
				for(int32 Index=0; Index<FoliageInfo->Instances.Num(); ++Index)
				{
					Indices_List.Add(Index);
				}
				FoliageInfo->RemoveInstances(Indices_List, true);
			}
		}
	}
}

UTexture2D* AProjectKR_LandscapeGenerator::SaveArrayToTexture(const FString& InAssetName, int32 InSizeX, int32 InSizeY, const TArray<FColor>& InPixel_List)
{
	FString PackagePath = GetTextureFilePath() + InAssetName;
	if(UPackage* Package = CreatePackage(*PackagePath))
	{
		Package->FullyLoad();
		
		UTexture2D* Texture = FindObject<UTexture2D>(Package, *InAssetName);
		if(Texture == nullptr)
		{
			Texture = NewObject<UTexture2D>(Package, *InAssetName, RF_Public | RF_Standalone);
		}

		FTexturePlatformData* PlatformData = Texture->GetPlatformData();
		if(PlatformData == nullptr)
		{
			PlatformData = new FTexturePlatformData();
		}
		PlatformData->SizeX = InSizeX;
		PlatformData->SizeY = InSizeY;
		PlatformData->PixelFormat = EPixelFormat::PF_B8G8R8A8; // FColor
		PlatformData->Mips.Empty();
		
		Texture->SetPlatformData(PlatformData);

		FTexture2DMipMap* MipMap = new FTexture2DMipMap();
		PlatformData->Mips.Add(MipMap);
		MipMap->SizeX = InSizeX;
		MipMap->SizeY = InSizeY;

		MipMap->BulkData.Lock(LOCK_READ_WRITE);
		{
			void* Data = MipMap->BulkData.Realloc(InPixel_List.Num() * sizeof(FColor));
			FMemory::Memcpy(Data, InPixel_List.GetData(), InPixel_List.Num() * sizeof(FColor));
		}
		MipMap->BulkData.Unlock();
		
		Texture->Source.Init(InSizeX, InSizeY, 1, 1, ETextureSourceFormat::TSF_BGRA8, reinterpret_cast<const uint8*>(InPixel_List.GetData()));

		Texture->SRGB = false;
		Texture->CompressionSettings = TextureCompressionSettings::TC_Default;
		Texture->UpdateResource();
		Texture->PostEditChange();

		Package->MarkPackageDirty();
		FAssetRegistryModule::AssetCreated(Texture);

		TArray<UPackage*> PackagesToSave;
		PackagesToSave.Add(Package);

		UEditorLoadingAndSavingUtils::SavePackages(PackagesToSave, true);

		return Texture;
	}
	
	return nullptr;
}
bool AProjectKR_LandscapeGenerator::LoadArrayFromTexture(UTexture2D* InTexture, TArray<FColor>& OutPixel_List) const
{
	if(InTexture == nullptr)
		return false;

	if(InTexture->Source.IsValid() == true)
	{
		TArray64<uint8> MipData_List;
		InTexture->Source.GetMipData(MipData_List, 0);

		const int32 PixelCount = InTexture->GetSizeX() * InTexture->GetSizeY();
		if(MipData_List.Num() != PixelCount * static_cast<int64>(sizeof(FColor)))
			return false;

		OutPixel_List.SetNumUninitialized(PixelCount);

		FMemory::Memcpy(OutPixel_List.GetData(), MipData_List.GetData(), PixelCount * sizeof(FColor));
		return true;
	}

	if(FTexturePlatformData* PlatformData = InTexture->GetPlatformData();
	PlatformData != nullptr && PlatformData->Mips.Num() > 0)
	{
		const int32 SizeX = InTexture->GetSizeX();
		const int32 SizeY = InTexture->GetSizeY();
		OutPixel_List.SetNumUninitialized(SizeX * SizeY);
		if(const void* MipData = PlatformData->Mips[0].BulkData.LockReadOnly())
		{
			FMemory::Memcpy(OutPixel_List.GetData(), MipData, SizeX * SizeY * sizeof(FColor));
			PlatformData->Mips[0].BulkData.Unlock();
			return true;
		}
	}
	return false;
}

FVector2D AProjectKR_LandscapeGenerator::PixelToWorld(const int32 InPixelX, const int32 InPixelY) const
{
	const FVector Origin = (ManagedLandscape.IsValid() == true) ? ManagedLandscape->GetActorLocation() : GetActorLocation();

	return FVector2D(Origin.X + static_cast<float>(InPixelX) * 100.0f, Origin.Y + static_cast<float>(InPixelY) * 100.0f);
}

void AProjectKR_LandscapeGenerator::BroadcastNotification(const FString& InNotificationString) const
{
	FNotificationInfo NotificationInfo(FText::FromString(InNotificationString));
	NotificationInfo.ExpireDuration = 4.0f;
	NotificationInfo.FadeOutDuration = 1.0f;
	NotificationInfo.bFireAndForget = true;
	NotificationInfo.bUseSuccessFailIcons = true;
	
	TSharedPtr<SNotificationItem> Item = FSlateNotificationManager::Get().AddNotification(NotificationInfo);

	Item->SetCompletionState(SNotificationItem::CS_Fail);
}

void AProjectKR_LandscapeGenerator::SmoothHeightMap(TArray<FColor>& InOutPixel_List, int32 InSizeX, int32 InSizeY) const
{
	static const int32 GaussianBlur_List[5] = { 1, 4, 6, 4, 1 };
	static constexpr int32 GaussianBlurSum = 16;
	static constexpr int32 GaussianBlurRadius = 2;

	const int32 PixelCount = InSizeX * InSizeY;

	TArray<uint8> HorizontalBlurResult_List;
	HorizontalBlurResult_List.SetNumUninitialized(PixelCount);

	// for(int32 Index_Y=0; Index_Y<InSizeY; ++Index_Y)
	// {
	// 	for(int32 Index_X=0; Index_X<InSizeX; ++Index_X)
	// 	{
	// 		int32 Accumulate = 0;
	// 		for(int32 BlurOffset=-GaussianBlurRadius; BlurOffset<=GaussianBlurRadius; ++BlurOffset)
	// 		{
	// 			const int32 SampledX = FMath::Clamp(Index_X + BlurOffset, 0, InSizeX - 1);
	// 			const int32 BlurWeight = GaussianBlur_List[BlurOffset + GaussianBlurRadius];
	// 			Accumulate += static_cast<int32>(InOutPixel_List[Index_Y * InSizeX + SampledX].R) * BlurWeight;
	// 		}
	//
	// 		HorizontalBlurResult_List[Index_Y * InSizeX + Index_X] = static_cast<uint8>(Accumulate / GaussianBlurSum);
	// 	}
	// }
	
	// for(int32 Index_Y=0; Index_Y<InSizeY; ++Index_Y)
	// {
	// 	for(int32 Index_X=0; Index_X<InSizeX; ++Index_X)
	// 	{
	// 		int32 Accumulate = 0;
	// 		for(int32 BlurOffset=-GaussianBlurRadius; BlurOffset<=GaussianBlurRadius; ++BlurOffset)
	// 		{
	// 			const int32 SampledY = FMath::Clamp(Index_Y + BlurOffset, 0, InSizeY - 1);
	// 			const int32 BlurWeight = GaussianBlur_List[BlurOffset + GaussianBlurRadius];
	// 			Accumulate += static_cast<int32>(HorizontalBlurResult_List[SampledY * InSizeX + Index_X]) * BlurWeight;
	// 		}
	//
	// 		const uint8 BlurredHeight = static_cast<uint8>(Accumulate / GaussianBlurSum);
	// 		InOutPixel_List[Index_Y * InSizeX + Index_X] = FColor(BlurredHeight, BlurredHeight, BlurredHeight, 255);
	// 	}
	// }

	ParallelFor(InSizeY, [&](int32 Index_Y)
	{
		for(int32 Index_X=0; Index_X<InSizeX; ++Index_X)
		{
			int32 Accumulate = 0;
			for(int32 BlurOffset=-GaussianBlurRadius; BlurOffset<=GaussianBlurRadius; ++BlurOffset)
			{
				const int32 SampledX = FMath::Clamp(Index_X + BlurOffset, 0, InSizeX - 1);
				const int32 BlurWeight = GaussianBlur_List[BlurOffset + GaussianBlurRadius];
				Accumulate += static_cast<int32>(InOutPixel_List[Index_Y * InSizeX + SampledX].R) * BlurWeight;
			}
	
			HorizontalBlurResult_List[Index_Y * InSizeX + Index_X] = static_cast<uint8>(Accumulate / GaussianBlurSum);
		}
	});

	ParallelFor(InSizeY, [&](int32 Index_Y)
	{
		for(int32 Index_X=0; Index_X<InSizeX; ++Index_X)
		{
			int32 Accumulate = 0;
			for(int32 BlurOffset=-GaussianBlurRadius; BlurOffset<=GaussianBlurRadius; ++BlurOffset)
			{
				const int32 SampledY = FMath::Clamp(Index_Y + BlurOffset, 0, InSizeY - 1);
				const int32 BlurWeight = GaussianBlur_List[BlurOffset + GaussianBlurRadius];
				Accumulate += static_cast<int32>(HorizontalBlurResult_List[SampledY * InSizeX + Index_X]) * BlurWeight;
			}

			const uint8 BlurredHeight = static_cast<uint8>(Accumulate / GaussianBlurSum);
			InOutPixel_List[Index_Y * InSizeX + Index_X] = FColor(BlurredHeight, BlurredHeight, BlurredHeight, 255);
		}
	});
}

EProjectKR_LandscapeBiomeType AProjectKR_LandscapeGenerator::FindDominantBiome(const TMap<EProjectKR_LandscapeBiomeType, float>& InWeightMap)
{
	EProjectKR_LandscapeBiomeType DominantBiome = EProjectKR_LandscapeBiomeType::None;
	float MaxWeight = -1.0f;
	for(const TPair<EProjectKR_LandscapeBiomeType, float>& Pair : InWeightMap)
	{
		if(Pair.Value > MaxWeight)
		{
			MaxWeight     = Pair.Value;
			DominantBiome = Pair.Key;
		}
	}
	return DominantBiome;
}
