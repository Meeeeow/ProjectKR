// Fill out your copyright notice in the Description page of Project Settings.

#include "SeedExt_Sun.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"

#include "Kismet/GameplayStatics.h"

ASeedExt_Sun::ASeedExt_Sun()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
 
	// DirectionalLight
	SunDirectionalLightComponent = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("SunDirectionalLight"));
	SunDirectionalLightComponent->SetupAttachment(Root);
	SunDirectionalLightComponent->Intensity = MaxSunlightLux;
	SunDirectionalLightComponent->LightColor = FColor::White;
	SunDirectionalLightComponent->bAtmosphereSunLight = true;
	SunDirectionalLightComponent->AtmosphereSunLightIndex = 0;
	SunDirectionalLightComponent->CastShadows = true;
	SunDirectionalLightComponent->DynamicShadowDistanceMovableLight = 50000.0f;
	SunDirectionalLightComponent->CascadeDistributionExponent = 4.0f;
	SunDirectionalLightComponent->DynamicShadowCascades = 4;
	SunDirectionalLightComponent->SetForwardShadingPriority(1);

	MoonDirectionalLightComponent = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("MoonDirectionalLight"));
	MoonDirectionalLightComponent->SetupAttachment(Root);
	MoonDirectionalLightComponent->Intensity = 0.0f;
	MoonDirectionalLightComponent->LightColor = FColor(179, 204, 255);
	MoonDirectionalLightComponent->bAtmosphereSunLight = false;
	MoonDirectionalLightComponent->CastShadows = false;
	MoonDirectionalLightComponent->DynamicShadowDistanceMovableLight = 0.f;
	MoonDirectionalLightComponent->SetForwardShadingPriority(0);
	
	// SkyLight
	SkyLightComponent = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
	SkyLightComponent->SetupAttachment(Root);
	SkyLightComponent->bRealTimeCapture = true;
	SkyLightComponent->SourceType = ESkyLightSourceType::SLS_CapturedScene;
	SkyLightComponent->Intensity = 1.0f;
 
	// SkyAtmosphere
	SkyAtmosphereComponent = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
	SkyAtmosphereComponent->SetupAttachment(Root);
	SkyAtmosphereComponent->RayleighScatteringScale = 1.0f;
	SkyAtmosphereComponent->MieScatteringScale = 0.1f;
 
	// ExponentialHeightFog
	ExponentialHeightFogComponent = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("HeightFog"));
	ExponentialHeightFogComponent->SetupAttachment(Root);
	ExponentialHeightFogComponent->FogDensity = 0.008f;
	ExponentialHeightFogComponent->FogHeightFalloff = 0.2f;
	ExponentialHeightFogComponent->FogMaxOpacity = 0.9f;
 
	// Static Mesh
	SunStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SunStaticMesh"));
	SunStaticMeshComponent->SetupAttachment(Root);
	SunStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SunStaticMeshComponent->SetCastShadow(false);
	SunStaticMeshComponent->SetAbsolute(true, true, false);

	MoonStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MoonStaticMesh"));
	MoonStaticMeshComponent->SetupAttachment(Root);
	MoonStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MoonStaticMeshComponent->SetCastShadow(false);
	MoonStaticMeshComponent->SetAbsolute(true, true, false);
	MoonStaticMeshComponent->SetVisibility(false);

	// PostProcess
	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	PostProcessComponent->SetupAttachment(Root);
	PostProcessComponent->bUnbound = true;
	PostProcessComponent->Priority = -1;
}

// Called when the game starts or when spawned
void ASeedExt_Sun::BeginPlay()
{
	Super::BeginPlay();

	if(SunDirectionalLightComponent != nullptr)
		SunDirectionalLightComponent->SetForwardShadingPriority(1);
	if(MoonDirectionalLightComponent != nullptr)
		MoonDirectionalLightComponent->SetForwardShadingPriority(0);

	ApplyPostProcessSettings();
	ComputeSunState();
	ComputeMoonState();
	ComputeBiomeInfluence();
	ApplyAtmosphereParams(ComputeAtmosphereParams());
}

void ASeedExt_Sun::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyPostProcessSettings();
	ComputeSunState();
	ComputeMoonState();
	ComputeBiomeInfluence();
	ApplyAtmosphereParams(ComputeAtmosphereParams());
}

// Called every frame
void ASeedExt_Sun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DayTime = FMath::Fmod(DayTime + DeltaTime * DayTimeScale / 86400.f, 1.f);
	LunarCycle = FMath::Fmod(LunarCycle + DeltaTime * DayTimeScale / (86400.f * 29.35f), 1.f);
	SeasonCycle = FMath::Fmod(SeasonCycle + DeltaTime * SeasonCycleScale / (86400.f * 365.25f), 1.f);

	const bool bWasDayTime = CurrentSunState.IsDayTime();
	ComputeSunState();
	ComputeMoonState();
	ApplyAtmosphereParams(ComputeAtmosphereParams());
	ApplyPostProcessSettings();

	const bool bIsDayTime = CurrentSunState.IsDayTime();
	if(bWasDayTime != bIsDayTime)
		OnSunriseDelegator.Broadcast(bIsDayTime);

	BiomeInfluenceAccum += DeltaTime;
	if(BiomeInfluenceAccum >= BiomeUpdateInterval)
	{
		BiomeInfluenceAccum = 0.0f;
		ComputeBiomeInfluence();
		OnInfluenceChangedDelegator.Broadcast(CurrentBiomeInfluence);
		
		if(CurrentSunState.SeasonType != PreviousSeasonType)
		{
			OnSeasonChangedDelegator.Broadcast(CurrentSunState.SeasonType, PreviousSeasonType);
			PreviousSeasonType = CurrentSunState.SeasonType;
		}
	}

	// const bool bIsCaptured = (LastSkyCaptureElevationAngle != -FLT_MAX);
	// const bool bIsCaptureSky = bIsCaptured==false || CurrentSunState.ElevationAngle>-12.f;
	// if(bIsCaptureSky == false)
	// 	return;
	//
	// if(const float ElevationDelta = FMath::Abs(CurrentSunState.ElevationAngle - LastSkyCaptureElevationAngle); ElevationDelta >= SkyRecaptureThresholdDegree)
	// {
	// 	SkyLightComponent->RecaptureSky();
	// 	LastSkyCaptureElevationAngle = CurrentSunState.ElevationAngle;
	// }
	
}

#if WITH_EDITOR
void ASeedExt_Sun::PostEditChangeProperty(FPropertyChangedEvent& InEvent)
{
	Super::PostEditChangeProperty(InEvent);
	ComputeSunState();
	ComputeMoonState();
	ComputeBiomeInfluence();
	ApplyAtmosphereParams(ComputeAtmosphereParams());

	TArray<UObject*> ExportObject_List;
	ExportObject_List.Add(SunDirectionalLightComponent);
	ExportObject_List.Add(MoonDirectionalLightComponent);
	ExportObject_List.Add(SkyLightComponent);
	ExportObject_List.Add(SkyAtmosphereComponent);
	ExportObject_List.Add(ExponentialHeightFogComponent);
	ExportObject_List.Add(SunStaticMeshComponent);
	ExportObject_List.Add(MoonStaticMeshComponent);
	ExportObject_List.Add(PostProcessComponent);
	ExportPropertyAdditiveObject(ExportObject_List);
}
#endif

void ASeedExt_Sun::SetDayTime(float InNewDayTime)
{
	DayTime = FMath::Clamp(InNewDayTime, 0.f, 1.f);
	ComputeSunState();
	ComputeMoonState();
	ComputeBiomeInfluence();
	ApplyAtmosphereParams(ComputeAtmosphereParams());
	OnInfluenceChangedDelegator.Broadcast(CurrentBiomeInfluence);
}
void ASeedExt_Sun::SetSeasonType(ESeedExt_SeasonType InNewSeasonType)
{
	switch(InNewSeasonType)
	{
		case ESeedExt_SeasonType::Spring: { SeasonCycle = 0.125f; break;}
		case ESeedExt_SeasonType::Summer: { SeasonCycle = 0.375f; break;}
		case ESeedExt_SeasonType::Autumn: { SeasonCycle = 0.625f; break;}
		case ESeedExt_SeasonType::Winter: { SeasonCycle = 0.875f; break;}
		default: return;
	}

	ComputeSunState();
	ComputeMoonState();
	ComputeBiomeInfluence();
	ApplyAtmosphereParams(ComputeAtmosphereParams());
	OnInfluenceChangedDelegator.Broadcast(CurrentBiomeInfluence);
	OnSeasonChangedDelegator.Broadcast(CurrentSunState.SeasonType, PreviousSeasonType);
}

FSeedExt_InfluenceState ASeedExt_Sun::GetBiomeInfluenceStateAtLocation(const FVector& InLocation) const
{
	FSeedExt_InfluenceState LocalState = CurrentBiomeInfluence;
	{
		const float AltKm = InLocation.Z / 100000.f;
		LocalState.TemperatureOffset -= AltKm * 0.16f;
		LocalState.HumidityOffset -= FMath::Clamp(AltKm * 0.05f, 0.f, 0.3f);
	}

	if(InfluenceZone_List.IsEmpty() == false)
	{
		const FVector2D XY(InLocation.X, InLocation.Y);
		for(const FSeedExt_InfluenceZone& InfluenceZone : InfluenceZone_List)
		{
			if(InfluenceZone.WorldRadius < KINDA_SMALL_NUMBER)
				continue;

			const float Distance = FVector2D::Distance(XY, InfluenceZone.WorldCenter);
			if(Distance >= InfluenceZone.WorldRadius)
				continue;

			const float InnerRadius = InfluenceZone.WorldRadius * FMath::Clamp(InfluenceZone.FallOffFraction, 0.f, 1.f);
			const float Weight = (Distance <= InnerRadius) ? 1.f : 1.f - (Distance - InnerRadius) / FMath::Max(InfluenceZone.WorldRadius, KINDA_SMALL_NUMBER);

			LocalState.TemperatureOffset += InfluenceZone.TemperatureDelta * Weight;
			LocalState.HumidityOffset += InfluenceZone.HumidityDelta * Weight;
		}
	}
	
	return LocalState;
}

void ASeedExt_Sun::ComputeSunState()
{
	/** 태양의 적위를 구하는 공식 ( Declination =  -23.45 × cos(2π × (n/365 + 0.0833) ) */
	const float DeclinationDegree = -23.45f * FMath::Cos(2.0f * PI * (SeasonCycle + 0.0833f));

	/** 관측자 ( Project KR = Korea = 35 Degree )의 위도를 Radian으로 변환 */ 
	const float LatitudeRadian = FMath::DegreesToRadians(LatitudeDegree);
	/** 태양의 적위(DeclinationDegree)를 Radian으로 변환 */
	const float DeclinationRadian = FMath::DegreesToRadians(DeclinationDegree);

	/** 시간 각을 계산함 DayTime이 0~1로 정규화 되어 있기 때문에 -180 ~ +180도로 변환 */
	const float HourAngleDegree = (DayTime - 0.5f) * 360.0f;
	/** 계산한 시간 각(HourAngleDegree)을 Radian으로 변환 */
	const float HourAngleRadian = FMath::DegreesToRadians(HourAngleDegree);

	/**
	 * 구면 삼각법 ( https://en.wikipedia.org/wiki/Spherical_trigonometry )을 이용한 태양 고도의 sin 값 산출
	 * sin(Elevation) = sin(Latitude)sin(Declination) + cos(Latitude)cos(Declination)cos(HourAngle)
	 */
	const float ElevationSinValue = FMath::Sin(LatitudeRadian) * FMath::Sin(DeclinationRadian) + FMath::Cos(LatitudeRadian) * FMath::Cos(DeclinationRadian) * FMath::Cos(HourAngleRadian);
	/** arc sin을 통해 SinValue를 고도 값을 Radian으로 추출 */
	const float ElevationRadian = FMath::Asin(FMath::Clamp(ElevationSinValue, -1.0f, 1.0f));
	/** 최종적으로 태양의 고도각 산출 */
	const float ElevationDegree = FMath::RadiansToDegrees(ElevationRadian);

	/** 방위각 계산. 천문학 관례상 초기 값 남쪽을 0이나 180으로 잡는다고 하는데, 아래 조건문 때문에 180으로 가정 */
	float AzimuthDegree = 180.0f;
	const float ElevationCosValue = FMath::Cos(ElevationRadian);
	if(ElevationCosValue > KINDA_SMALL_NUMBER)
	{
		/**
		 * 태양의 방위각을 구하기 위한 구면 삼각법
		 * 이미 고도각을 통해 고도를 알 수 있기 때문에, 태양의 방위 ( 동,서,남,북 )를 구해야함
		 * cos(Azimuth) = sin(Declination) - sin(Latitude)sin(Elevation) / cos(Latitude)cos(Elevation)
		 */
		const float AzimuthCosValue = (FMath::Sin(DeclinationRadian) - FMath::Sin(LatitudeRadian) * ElevationSinValue) / (ElevationCosValue * FMath::Cos(LatitudeRadian));
		AzimuthDegree = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(AzimuthCosValue, -1.0f, 1.0f)));
		// 오후( > 0.5)면 서쪽으로 이동 해야함. Acos함수는 0~180만 반환하기 때문에, 360도 대칭으로 서쪽 방향으로 설정  
		if(DayTime > 0.5f)
		{
			AzimuthDegree = 360.0f - AzimuthDegree;
		}
	}

	/**
	 * 낮 길이 계산 ( 해당 위도와 계절에서 해가 떠 있는 시간의 비율 )
	 * cons(HourAngle) = -tan(Latitude)tan(Declination)
	 */
	const float HourAngleCosValue = -FMath::Tan(LatitudeRadian) * FMath::Tan(DeclinationRadian);
	
	float DaylightFraction = 0.5f;  // 기본: 12시간
	if(FMath::Abs(HourAngleCosValue) <= 1.0f)
	{
		const float HourAngleSunriseDegree = FMath::RadiansToDegrees(FMath::Acos(HourAngleCosValue));
		DaylightFraction = HourAngleSunriseDegree / 180.0f;
	}
	else
	{
		// 극야(HourAngleCosValue > 1) or 백야(HourAngleCosValue < -1)
		DaylightFraction = (HourAngleCosValue > 1.0f) ? 0.0f : 1.0f;
	}
	
	/**
	 * 대기 감쇠 계산
	 */
	float NormalizedIntensity = 0.0f;
	if(ElevationDegree > 0.0f)
	{
		/**
		 * Kasten-Young 공식 근사: airmass = 1 / (sin(Elev) + 0.50572*(Elev+6.07995)^-1.6364)
		 */
		const float AirMass = 1.0f / (ElevationSinValue + 0.50572f * FMath::Pow(ElevationDegree + 6.07995f, -1.6364f));
		/**
		 * 대기 투과율 (지수 감쇠): T^AirMass, T = 0.7 (평균 대기 투명도)
		 */
		NormalizedIntensity = FMath::Clamp(FMath::Pow(0.7f, AirMass) * ElevationSinValue, 0.0f, 1.0f);
	}

	/**
	 * 고도각에 따른 색 온도 계산
	 * 낮은 고도 ( 일출 / 일몰 ) = 2000 ~ 3000 K
	 * 높은 고도 ( 정오 ) = 5800 ~ 6500 K
	 */
	float ColorTemperatureK = 0.0f;
	if(ElevationDegree <= 0.0f)
		ColorTemperatureK = 2000.0f;
	else if(ElevationDegree < 10.0f)
		ColorTemperatureK = FMath::Lerp(2200.0f, 4000.0f, ElevationDegree / 10.0f);
	else if(ElevationDegree < 30.0f)
		ColorTemperatureK = FMath::Lerp(4000.0f, 5500.0f, (ElevationDegree - 10.0f) / 20.0f);
	else
		ColorTemperatureK = FMath::Lerp(5500.0f, 6200.0f, FMath::Min((ElevationDegree - 30.0f) / 60.0f, 1.0f));
	
	// 현재 계절 선택
	ESeedExt_SeasonType Season;
	if(SeasonCycle < 0.25f)			Season = ESeedExt_SeasonType::Spring;
	else if(SeasonCycle < 0.50f)	Season = ESeedExt_SeasonType::Summer;
	else if(SeasonCycle < 0.75f)	Season = ESeedExt_SeasonType::Autumn;
	else							Season = ESeedExt_SeasonType::Winter;
	
	// 현재 태양의 상태를 업데이트
	CurrentSunState.ElevationAngle			= ElevationDegree;
	CurrentSunState.Azimuth					= AzimuthDegree;
	CurrentSunState.NormalizedIntensity		= NormalizedIntensity;
	CurrentSunState.DayTime					= DayTime;
	CurrentSunState.SeasonCycle				= SeasonCycle;
	CurrentSunState.SeasonType				= Season;
	CurrentSunState.ColorKelvinTemperature	= ColorTemperatureK;
	CurrentSunState.DayLightFraction		= DaylightFraction;
	
	// 각도 설정
	const float Pitch = -ElevationDegree; 
	const float Yaw = AzimuthDegree - 180.0f;
	SunDirectionalLightComponent->SetWorldRotation(FRotator(Pitch, Yaw, 0.0f));

	// 위치 설정
	if(SunStaticMeshComponent!=nullptr && SunStaticMeshComponent->GetStaticMesh()!=nullptr)
	{
		const bool bIsVisible = CurrentSunState.ElevationAngle > -2.0f;
		SunStaticMeshComponent->SetVisibility(bIsVisible);

		if(bIsVisible == false)
			return;
		
		FVector CameraPosition = GetActorLocation();
		if(const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FVector CameraLocation = FVector::ZeroVector;
			FRotator CameraRotation = FRotator::ZeroRotator;
			PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
			CameraPosition = CameraLocation;
		}
		
		const FVector SunDirection = -SunDirectionalLightComponent->GetForwardVector();
		const FVector SunWorldPosition = CameraPosition + SunDirection * AstronomicalUnit;
		SunStaticMeshComponent->SetWorldLocation(SunWorldPosition);

		float ScaleMultiplier = FMath::Lerp(1.5f, 1.0f, FMath::Clamp(CurrentSunState.ElevationAngle / 90.0f, 0.0f, 1.0f));
		SunStaticMeshComponent->SetWorldScale3D(FVector(ScaleMultiplier));
	}
	
}
void ASeedExt_Sun::ComputeMoonState()
{
	const float MoonPhase = FMath::Abs(FMath::Sin(LunarCycle * PI));
	const float MoonDayTime = FMath::Fmod(DayTime + MoonPhase * 0.5f, 1.f);

	const float LatitudeRadian = FMath::DegreesToRadians(LatitudeDegree);
	const float DeclinationDegree = -23.45f * FMath::Cos(2.0f * PI * (SeasonCycle + 0.0833f));
	const float DeclinationRadian = FMath::DegreesToRadians(DeclinationDegree);
	const float HourAngleRadian = FMath::DegreesToRadians((MoonDayTime - 0.5f) * 360.f);

	const float ElevationSinValue = FMath::Sin(LatitudeRadian) * FMath::Sin(DeclinationRadian) + FMath::Cos(LatitudeRadian) * FMath::Cos(DeclinationRadian) * FMath::Cos(HourAngleRadian);
	const float ElevationRadian = FMath::Asin(FMath::Clamp(ElevationSinValue, -1.f, 1.f));
	const float ElevationDegree = FMath::RadiansToDegrees(ElevationRadian);

	float AzimuthDegree = 180.f;
	const float ElevationCosValue = FMath::Cos(ElevationRadian);
	if(ElevationCosValue > KINDA_SMALL_NUMBER)
	{
		const float AzimuthCosValue = (FMath::Sin(DeclinationRadian) - FMath::Sin(LatitudeRadian) * ElevationSinValue) / (ElevationCosValue * FMath::Cos(LatitudeRadian));
		AzimuthDegree = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(AzimuthCosValue, -1.f, 1.f)));
		if(MoonDayTime > 0.5f)
			AzimuthDegree = 360.f - AzimuthDegree;
	}

	const float MoonHorizonBlend = FMath::Clamp(ElevationDegree/3.f, 0.1f, 1.f);
	const float LightIntensity = MoonPhase * MoonlightIntensity * 75000.f * MoonHorizonBlend;

	CurrentMoonState.ElevationAngle = ElevationDegree;
	CurrentMoonState.Azimuth = AzimuthDegree;
	CurrentMoonState.MoonPhase = MoonPhase;
	CurrentMoonState.LightIntensity = LightIntensity;

	const float Pitch = -ElevationDegree; 
	const float Yaw = AzimuthDegree - 180.0f;
	MoonDirectionalLightComponent->SetWorldRotation(FRotator(Pitch, Yaw, 0.f));

	if(MoonStaticMeshComponent!=nullptr && MoonStaticMeshComponent->GetStaticMesh()!=nullptr)
	{
		const bool bIsVisible = LightIntensity > KINDA_SMALL_NUMBER;
		MoonStaticMeshComponent->SetVisibility(bIsVisible);

		if(bIsVisible == false)
			return;

		FVector CameraPosition = GetActorLocation();
		if(const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FVector CameraLocation = FVector::ZeroVector;
			FRotator CameraRotation = FRotator::ZeroRotator;
			PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
			CameraPosition = CameraLocation;
		}
		
		const FVector MoonDirection = -MoonDirectionalLightComponent->GetForwardVector();
		const FVector MoonWorldPosition = CameraPosition + MoonDirection * AstronomicalUnit;
		MoonStaticMeshComponent->SetWorldLocation(MoonWorldPosition);

		float ScaleMultiplier = FMath::Lerp(1.5f, 1.0f, FMath::Clamp(CurrentMoonState.ElevationAngle / 90.0f, 0.0f, 1.0f));
		MoonStaticMeshComponent->SetWorldScale3D(FVector(ScaleMultiplier));
	}
}
void ASeedExt_Sun::ComputeBiomeInfluence()
{
	const float MaxNoonElev = FMath::Clamp(90.f - FMath::Abs(LatitudeDegree) + 23.45f, 0.f, 90.f);
	CurrentBiomeInfluence.TemperatureOffset = FMath::Lerp(-0.45f, 0.45f, MaxNoonElev / 90.f);
	CurrentBiomeInfluence.LatitudeBandType = CurrentSunState.GetLatitudeBandType();

	CurrentBiomeInfluence.SeasonalWarmth = FMath::Clamp(0.5f -0.5f * FMath::Cos(2.f * PI * (CurrentSunState.SeasonCycle - 0.25f)), 0.f, 1.f);

	CurrentBiomeInfluence.TemperatureMultiplier = FMath::Lerp(0.5f, 1.5f, CurrentSunState.DayLightFraction);

	switch(CurrentSunState.SeasonType)
	{
		case ESeedExt_SeasonType::Summer:
		{
			CurrentBiomeInfluence.TemperatureMultiplier *= 1.2f;
			CurrentBiomeInfluence.TemperatureOffset += 0.1f;
			break;
		}
		case ESeedExt_SeasonType::Winter:
		{
			CurrentBiomeInfluence.TemperatureMultiplier *= 0.7f;
			CurrentBiomeInfluence.TemperatureOffset -= 0.15f;
			break;
		}
		default:
		{
			CurrentBiomeInfluence.TemperatureMultiplier *= 0.9f;
			break;
		}
	}

	CurrentBiomeInfluence.TemperatureMultiplier = FMath::Clamp(CurrentBiomeInfluence.TemperatureMultiplier, 0.1f, 2.f);
	CurrentBiomeInfluence.TemperatureOffset = FMath::Clamp(CurrentBiomeInfluence.TemperatureOffset, -0.5f, 0.5f);

	switch(CurrentBiomeInfluence.LatitudeBandType)
	{
		case ESeedExt_LatitudeBandType::Polar:
		{
			CurrentBiomeInfluence.HumidityOffset = -0.25f;
			CurrentBiomeInfluence.HumidityMultiplier = 0.5f;
			break;
		}
		case ESeedExt_LatitudeBandType::Subpolar:
		{
			CurrentBiomeInfluence.HumidityOffset = 0.05f;
			CurrentBiomeInfluence.HumidityMultiplier = 0.9f;
			break;
		}
		case ESeedExt_LatitudeBandType::Temperate:
		{
			CurrentBiomeInfluence.HumidityOffset = 0.0f;
			CurrentBiomeInfluence.HumidityMultiplier = 1.0f;
			break;
		}
		case ESeedExt_LatitudeBandType::SubTropical:
		{
			CurrentBiomeInfluence.HumidityOffset = -0.15f;
			CurrentBiomeInfluence.HumidityMultiplier = 0.7f;
			break;
		}
		case ESeedExt_LatitudeBandType::Tropical:
		{
			CurrentBiomeInfluence.HumidityOffset = 0.2f;
			CurrentBiomeInfluence.HumidityMultiplier = 1.3f;
			break;
		}
	}

	switch(CurrentSunState.SeasonType)
	{
		case ESeedExt_SeasonType::Spring:
		{
			CurrentBiomeInfluence.SeasonalPrecipitation = 1.3f;
			break;
		}
		case ESeedExt_SeasonType::Summer:
		{
			CurrentBiomeInfluence.SeasonalPrecipitation = (CurrentBiomeInfluence.LatitudeBandType == ESeedExt_LatitudeBandType::Tropical) ? 1.5f : 0.8f;
			break;
		}
		case ESeedExt_SeasonType::Autumn:
		{
			CurrentBiomeInfluence.SeasonalPrecipitation = 1.2f;
			break;
		}
		case ESeedExt_SeasonType::Winter:
		{
			CurrentBiomeInfluence.SeasonalPrecipitation = 0.6f;
			break;
		}
	}

	CurrentBiomeInfluence.AtmosphericClarity = FMath::Lerp(0.4f, 1.f, FMath::Clamp(CurrentSunState.ElevationAngle / 90.f, 0.f, 1.f));
	CurrentBiomeInfluence.DiurnalAplitude = (CurrentBiomeInfluence.LatitudeBandType == ESeedExt_LatitudeBandType::SubTropical || CurrentBiomeInfluence.LatitudeBandType == ESeedExt_LatitudeBandType::Polar) ? 0.25f : 0.1f;
}
FSeedExt_SunAtomsphereParams ASeedExt_Sun::ComputeAtmosphereParams() const
{
	FSeedExt_SunAtomsphereParams SunParams;

	SunParams.SunLightIntensity = CurrentSunState.NormalizedIntensity * MaxSunlightLux;
	if(CurrentSunState.ElevationAngle < 15.f)
		SunParams.SunLightColor = FMath::Lerp(SunriseColor, NoonColor, FMath::Max(CurrentSunState.ElevationAngle, 0.f) / 15.f);
	else
		SunParams.SunLightColor = FMath::Lerp(NoonColor * 0.9f, NoonColor, FMath::Clamp((CurrentSunState.ElevationAngle - 15.f) / 75.f, 0.f, 1.f));

	SunParams.MoonLightColor = MoonColor;
	SunParams.MoonLightIntensity = CurrentMoonState.LightIntensity;

	const float DaySkyLight = FMath::Clamp(CurrentSunState.NormalizedIntensity * 2.f, 0.f, 2.f);
	constexpr float StarAmbient = 0.05f;
	const float NightSkyLight = StarAmbient + FMath::Lerp(0.f, 0.2f, CurrentMoonState.MoonPhase);

	const float SkyLightTwilightBlend = FMath::Clamp((CurrentSunState.ElevationAngle + 15.f) / 20.f, 0.f, 1.f);
	//SunParams.SkyLightIntensity = ( bIsNight==true ) ? NightSkyLight : FMath::Max(DaySkyLight, 0.1f);
	SunParams.SkyLightIntensity = FMath::Lerp(NightSkyLight, FMath::Max(DaySkyLight, 0.1f), SkyLightTwilightBlend);

	if(CurrentSunState.ElevationAngle < -3.f)
	{
		SunParams.SkyColor = DeepNightSkyColor;
	}
	else if(CurrentSunState.ElevationAngle < 0.0f)
	{
		const float TwilightBlendFactor = (CurrentSunState.ElevationAngle + 3.f) / 3.f;
		SunParams.SkyColor = FMath::Lerp(DeepNightSkyColor, TwilightSkyColor, TwilightBlendFactor);
	}
	else if(CurrentSunState.ElevationAngle < 10.f)
	{
		const float SunriseBlendFactor = CurrentSunState.ElevationAngle / 10.f;
		SunParams.SkyColor = FMath::Lerp(TwilightSkyColor, DaySkyColor, SunriseBlendFactor);
	}
	else
	{
		SunParams.SkyColor = DaySkyColor;
	}

	SunParams.FogDensity = FMath::Lerp(0.008f, 0.015f, FMath::Clamp(CurrentSunState.ElevationAngle / 30.f, 0.f, 1.f));

	const float Clarity = CurrentBiomeInfluence.AtmosphericClarity;
	SunParams.MieScatteringScale = FMath::Lerp(0.5f, 0.05f, Clarity);

	return SunParams;
}
void ASeedExt_Sun::ApplyAtmosphereParams(const FSeedExt_SunAtomsphereParams& InParams)
{
	if(SunDirectionalLightComponent != nullptr)
	{
		SunDirectionalLightComponent->SetLightColor(InParams.SunLightColor);
		SunDirectionalLightComponent->SetIntensity(InParams.SunLightIntensity);
	}
	if(MoonDirectionalLightComponent != nullptr)
	{
		MoonDirectionalLightComponent->SetLightColor(InParams.MoonLightColor);
		MoonDirectionalLightComponent->SetIntensity(InParams.MoonLightIntensity);
	}

	if(SkyLightComponent != nullptr)
	{
		SkyLightComponent->SetIntensity(InParams.SkyLightIntensity);
	}

	if(SkyAtmosphereComponent != nullptr)
	{
		SkyAtmosphereComponent->SetRayleighScatteringScale(InParams.RayleighScattering);
		SkyAtmosphereComponent->SetMieScatteringScale(InParams.MieScatteringScale);
	}

	if(ExponentialHeightFogComponent != nullptr)
	{
		ExponentialHeightFogComponent->SetFogDensity(InParams.FogDensity);
		ExponentialHeightFogComponent->SetFogInscatteringColor(InParams.SkyColor);
	}
}
void ASeedExt_Sun::ApplyPostProcessSettings()
{
	if(PostProcessComponent == nullptr)
		return;

	FPostProcessSettings& PostProcessSettings = PostProcessComponent->Settings;

	PostProcessSettings.bOverride_AutoExposureMethod = true;
	PostProcessSettings.AutoExposureMethod = EAutoExposureMethod::AEM_Histogram;

	//const float NormalizedLux = FMath::Clamp(MaxSunlightLux / 75000.f, 0.01f, 1.f);
	const float NormalizedLux = FMath::Clamp(CurrentSunState.NormalizedIntensity, 0.01f, 1.f);

	const float NightExposureMin = FMath::Lerp(-8.f, -2.f, CurrentMoonState.MoonPhase);
	const float NightNormalizedLux = MoonlightIntensity * 75000.f;
	const float MoonMaxEV = FMath::Log2(FMath::Max(NightNormalizedLux, 1.f) / 12.5f);
	const float NightExposureMax = FMath::Lerp(-1.f, MoonMaxEV, CurrentMoonState.MoonPhase);

	const float DayExposureMin = FMath::Lerp(4.f, 6.f, NormalizedLux);
	const float DayExposureMax = FMath::Lerp(10.f, 14.f, NormalizedLux);

	const float DawnDuskBlendFactor = FMath::Clamp((CurrentSunState.ElevationAngle + 5.f)/25.f, 0.f, 1.f);

	const float ExposureMin = FMath::Lerp(NightExposureMin, DayExposureMin, DawnDuskBlendFactor);
	const float ExposureMax = FMath::Lerp(NightExposureMax, DayExposureMax, DawnDuskBlendFactor);

	PostProcessSettings.bOverride_AutoExposureMinBrightness = true;
	PostProcessSettings.bOverride_AutoExposureMaxBrightness = true;
	PostProcessSettings.AutoExposureMinBrightness = ExposureMin;
	PostProcessSettings.AutoExposureMaxBrightness = ExposureMax;
	
	PostProcessSettings.bOverride_AutoExposureSpeedUp = true;
	PostProcessSettings.bOverride_AutoExposureSpeedDown = true;
	PostProcessSettings.AutoExposureSpeedUp = 1.5f;
	PostProcessSettings.AutoExposureSpeedDown = 0.8f;
	
	PostProcessSettings.bOverride_FilmToe = false;
	PostProcessSettings.bOverride_BloomIntensity = true;

	PostProcessSettings.bOverride_WhiteTemp = true;
	const float KelvinBlend = FMath::Clamp(CurrentSunState.ElevationAngle / 30.f, 0.f, 1.f) * 0.25f;
	PostProcessSettings.WhiteTemp = FMath::Lerp(6500.f, CurrentSunState.ColorKelvinTemperature, KelvinBlend);
}

FLinearColor ASeedExt_Sun::ColorTemperatureToLinear(float InTemperature)
{
	InTemperature = FMath::Clamp(InTemperature, 1000.f, 15000.f);
	const float InvT = 1000.f / InTemperature, InvT2 = InvT * InvT, InvT3 = InvT2 * InvT;
	float x = (InTemperature < 4000.f)
		? (-0.2661239f*InvT3 - 0.2343589f*InvT2 + 0.8776956f*InvT + 0.179910f)
		: (-3.0258469f*InvT3 + 2.1070379f*InvT2 + 0.2226347f*InvT + 0.240390f);
	float x2=x*x, x3=x2*x;
	float y = (InTemperature < 2222.f)
		? (-1.1063814f*x3 - 1.3481102f*x2 + 2.18555832f*x - 0.20219683f)
		: ((InTemperature < 4000.f)
			? (-0.9549476f*x3 - 1.37418593f*x2 + 2.09137015f*x - 0.16748867f)
			: ( 3.0817580f*x3 - 5.87338670f*x2 + 3.75112997f*x - 0.37001483f));
	float z = 1.f - x - y, Y=1.f;
	float X = (y > 0.f) ? Y*x/y : 0.f;
	float Z = (y > 0.f) ? Y*z/y : 0.f;
	float R = FMath::Clamp( 3.2406f*X - 1.5372f*Y - 0.4986f*Z, 0.f, 1.f);
	float G = FMath::Clamp(-0.9689f*X + 1.8758f*Y + 0.0415f*Z, 0.f, 1.f);
	float B = FMath::Clamp( 0.0557f*X - 0.2040f*Y + 1.0570f*Z, 0.f, 1.f);
	const float M = FMath::Max3(R,G,B);
	if(M > KINDA_SMALL_NUMBER)
	{
		R/=M;
		G/=M;
		B/=M;
	}
	
	return FLinearColor(R,G,B);
}

