#include "ProjectKR_LandscapeGenerator.h"

#include <map>

#include "ClearReplacementShaders.h"
#include "Landscape.h"
#include "LandscapeImportHelper.h"
#include "LandscapeLayerInfoObject.h"
#include "Editor.h"
#include "EditorModeManager.h"
#include "EditorModes.h"
#include "EngineUtils.h"
#include "FileHelpers.h"
#include "LandscapeSubsystem.h"

#include "AssetRegistry/AssetRegistryModule.h"

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

				// FName DefaultLayerName = FName("Dirt");
				//
				// const float TemperatureValue = UProjectKR_LandscapeFunctionLibrary::GetTerrainHeight(Index_Width, Index_Height, ClimateNoiseScale, 2, Persistence, Lacunarity, TemperatureSeed);
				// const float HumidityValue = UProjectKR_LandscapeFunctionLibrary::GetTerrainHeight(Index_Width, Index_Height, ClimateNoiseScale, 2, Persistence, Lacunarity, HumiditySeed);
				//
				// float FinalTemperatureValue = (TemperatureValue * 1.5f + 1.0f) * 0.5f;
				// FinalTemperatureValue = FMath::Clamp(FinalTemperatureValue - (NormalizedNoiseValue * 0.1f), 0.0f, 1.0f);
				//
				// float FinalHumidityValue = (HumidityValue * 1.5f + 1.0f) * 0.5f;
				// FinalHumidityValue = FMath::Clamp(FinalHumidityValue, 0.0f, 1.0f);
				//
				// float MinDistance = 100.f;
				// for(TMap<EProjectKR_LandscapeBiomeType, FProjectKR_BiomeInfo>::TConstIterator It(LandscapeBiomeInfo_List); It; ++It)
				// {
				// 	const FProjectKR_BiomeEnvironment& BiomeInfo = It->Value.BiomeInfo;
				// 	if(const float Distance = FVector2D::Distance(FVector2D(FinalTemperatureValue, FinalHumidityValue), FVector2D(BiomeInfo.Temperature, BiomeInfo.Humidity)); Distance <  MinDistance)
				// 	{
				// 		MinDistance = Distance;
				// 		DefaultLayerName = BiomeInfo.BiomeName;
				// 	}
				// }
				//
				// LandscapeData_List[DefaultLayerName][Index] = 255;

				TMap<FName, float> BiomeWeightsMap;
				CalculateBiomeWeights(Index_Width, Index_Height, NormalizedNoiseValue, BiomeWeightsMap);

				if(BiomeWeightsMap.IsEmpty() == false)
				{
					for(const TPair<FName, float>& WeightPair : BiomeWeightsMap)
					{
						LandscapeData_List[WeightPair.Key][Index] = static_cast<uint8>(WeightPair.Value * 255.f);
					}
				}
				else
				{
					LandscapeData_List[TEXT("Dirt")][Index] = 255;
				}
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
void AProjectKR_LandscapeGenerator::CalculateBiomeWeights(const float InX, const float InY, const float InNormalizedHeight, TMap<FName, float>& OutWeightsMap) const
{
	OutWeightsMap.Empty();

	const float TemperatureValue = UProjectKR_LandscapeFunctionLibrary::GetTemperatureAt(InX, InY, Seed, ClimateNoiseScale);
	const float HumidityValue = UProjectKR_LandscapeFunctionLibrary::GetHumidityAt(InX, InY, Seed, ClimateNoiseScale);

	float TotalWeight = 0.0f;

	for(TMap<EProjectKR_LandscapeBiomeType, FProjectKR_BiomeInfo>::TConstIterator It(LandscapeBiomeInfo_List); It; ++It)
	{
		const FProjectKR_BiomeEnvironment& Environment = It->Value.BiomeInfo;

		const float ClimateDistance = FVector2D::Distance(FVector2D(TemperatureValue, HumidityValue), FVector2D(Environment.Temperature, Environment.Humidity));
		const float ClimateFitValue = FMath::Max(0.0f, 1.0f - (ClimateDistance * 2.0f));

		float HeightFitValue = 0.0f;
		if(InNormalizedHeight >= Environment.MinHeight && InNormalizedHeight <= Environment.MaxHeight)
		{
			const float Center = (Environment.MinHeight + Environment.MaxHeight) * 0.5f;
			const float Range = (Environment.MaxHeight - Environment.MinHeight) * 0.5f;

			HeightFitValue = (Range > 0.0f) ? FMath::Exp(-FMath::Square(InNormalizedHeight - Center) / FMath::Square(Range)) : 1.0f;
		}

		if(const float FinalWeight = ClimateFitValue * HeightFitValue; FinalWeight > 0.01f)
		{
			OutWeightsMap.Emplace(Environment.BiomeName, FinalWeight);
			TotalWeight += FinalWeight;
		}

		if(TotalWeight > 0.0f)
		{
			for(TPair<FName, float>& WeightPair : OutWeightsMap)
			{
				WeightPair.Value /= TotalWeight;
			}
		}
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

void AProjectKR_LandscapeGenerator::BakeHeightMap()
{
	const int32 QuadsPerComponent = SectionsPerComponent * QuadsPerSection;
	const int32 SizeX = ComponentCountX * QuadsPerComponent + 1;
	const int32 SizeY = ComponentCountY * QuadsPerComponent + 1;

	TArray<FColor> Pixel_List;
	Pixel_List.SetNumZeroed(SizeX * SizeY);

	const int32 BakedHeightSeed = FMath::Rand(); 

	for(int32 Index_Y=0; Index_Y<SizeY; ++Index_Y)
	{
		for(int32 Index_X=0; Index_X<SizeX; Index_X++)
		{
			float Noise = UProjectKR_LandscapeFunctionLibrary::GetTerrainHeight(Index_X, Index_Y, MicroNoiseScale, Octaves, Persistence, Lacunarity, BakedHeightSeed);
			float NormalizedHeight = (Noise + 1.0f) * 0.5f;

			NormalizedHeight = FMath::Pow(NormalizedHeight, RedistributionFactor);

			uint8 HeightByte = static_cast<uint8>(FMath::Clamp(NormalizedHeight*255.f, 0.f, 255.f));
			Pixel_List[Index_Y * SizeX + Index_X] = FColor(HeightByte, HeightByte, HeightByte, 255.f);
		}
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

		int32 Index = FCString::Atoi(*IndexPart);
		if(Index > MaxIndex)
		{
			MaxIndex = Index;
		}
	}

	FString TextureName = FString::Printf(TEXT("Texture_BakedHeightMap_%d"), ++MaxIndex);

	BakedHeightMap = SaveArrayToTexture(TextureName, SizeX, SizeY, Pixel_List);
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

	TArray<FColor> TemperaturePixel_List, HumidityPixel_List, BiomePixel_List;
	TemperaturePixel_List.SetNumUninitialized(SizeX * SizeY);
	HumidityPixel_List.SetNumUninitialized(SizeX * SizeY);
	BiomePixel_List.SetNumUninitialized(SizeX * SizeY);

	for(int32 Index_Y=0; Index_Y<SizeY; Index_Y++)
	{
		for(int32 Index_X=0; Index_X<SizeX; Index_X++)
		{
			int32 Index = Index_Y * SizeX + Index_X;

			float Height = HeightPixel_List[Index].R / 255.f;

			
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

		Texture->SRGB = true;
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
bool AProjectKR_LandscapeGenerator::LoadArrayFromTexture(UTexture2D* InTexture, TArray<FColor>& OutPixel_List)
{
	if(InTexture == nullptr)
	{
		return false;
	}

	if(InTexture->Source.IsValid() == true)
	{
		TArray64<uint8> MipData_List;
		InTexture->Source.GetMipData(MipData_List, 0);
		for(const uint8& MipData : MipData_List)
		{
			OutPixel_List.Emplace(MipData);
		}
		return true;
	}

	if(FTexturePlatformData* PlatformData = InTexture->GetPlatformData(); PlatformData != nullptr && PlatformData->Mips.Num() > 0)
	{
		int32 SizeX = InTexture->GetSizeX();
		int32 SizeY = InTexture->GetSizeY();
		OutPixel_List.SetNumUninitialized(SizeX * SizeY);

		if(const void* MipData = PlatformData->Mips[0].BulkData.LockReadOnly(); MipData != nullptr)
		{
			FMemory::Memcpy(OutPixel_List.GetData(), MipData, SizeX * SizeY * sizeof(FColor));
			PlatformData->Mips[0].BulkData.Unlock();
			return true;
		}
	}

	return false;
}
