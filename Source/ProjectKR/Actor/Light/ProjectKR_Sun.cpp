// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_Sun.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"

// Sets default values
AProjectKR_Sun::AProjectKR_Sun()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
 
	// ─── DirectionalLight ─────────────────────────────────────
	DirectionalLightComponent = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("DirectionalLight"));
	DirectionalLightComponent->SetupAttachment(Root);
	DirectionalLightComponent->Intensity = MaxSunlightLux;
	DirectionalLightComponent->LightColor = FColor::White;
	DirectionalLightComponent->bAtmosphereSunLight = true;
	DirectionalLightComponent->AtmosphereSunLightIndex = 0;
	DirectionalLightComponent->CastShadows = true;
	DirectionalLightComponent->DynamicShadowDistanceMovableLight = 50000.0f;
	DirectionalLightComponent->CascadeDistributionExponent = 4.0f;
	DirectionalLightComponent->DynamicShadowCascades = 4;
 
	// ─── SkyLight ─────────────────────────────────────────────
	SkyLightComponent = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
	SkyLightComponent->SetupAttachment(Root);
	SkyLightComponent->bRealTimeCapture = true;
	SkyLightComponent->SourceType = ESkyLightSourceType::SLS_CapturedScene;
	SkyLightComponent->Intensity = 1.0f;
 
	// ─── SkyAtmosphere ────────────────────────────────────────
	SkyAtmosphereComponent = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
	SkyAtmosphereComponent->SetupAttachment(Root);
	SkyAtmosphereComponent->RayleighScatteringScale = 1.0f;
	SkyAtmosphereComponent->MieScatteringScale = 1.0f;
 
	// ─── ExponentialHeightFog ─────────────────────────────────
	ExponentialHeightFogComponent = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("HeightFog"));
	ExponentialHeightFogComponent->SetupAttachment(Root);
	ExponentialHeightFogComponent->FogDensity = 0.02f;
	ExponentialHeightFogComponent->FogHeightFalloff = 0.2f;
	ExponentialHeightFogComponent->FogMaxOpacity = 0.9f;
 
	// ─── 태양 메시 (빌보드) ────────────────────────────────────
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SunMesh"));
	StaticMeshComponent->SetupAttachment(Root);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetCastShadow(false);
}

// Called when the game starts or when spawned
void AProjectKR_Sun::BeginPlay()
{
	Super::BeginPlay();

	ComputeSunState();
}

void AProjectKR_Sun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectKR_Sun::SetDayTime(float InNewDayTime)
{
	DayTime = FMath::Clamp(InNewDayTime, 0.0f, 1.0f);
	ComputeSunState();
	//ComputeBiomeInfluence();
	//ApplyAtmosphereParams(ComputeAtmosphereParams());
	OnBiomeInfluenceChanged(CurrentBiomeInfluence);
}

void AProjectKR_Sun::SetSeasonType(EProjectKR_SeasonType InNewSeasonType)
{
	switch(InNewSeasonType)
	{
		case EProjectKR_SeasonType::Spring: SeasonCycle = 0.125f; break;
		case EProjectKR_SeasonType::Summer: SeasonCycle = 0.375f; break;
		case EProjectKR_SeasonType::Autumn: SeasonCycle = 0.625f; break;
		case EProjectKR_SeasonType::Winter: SeasonCycle = 0.875f; break;
	}
	ComputeSunState();
	//ComputeBiomeInfluence();
	//ApplyAtmosphereParams(ComputeAtmosphereParams());
	OnBiomeInfluenceChanged(CurrentBiomeInfluence);
}

FProjectKR_SunInfluenceBiomeState AProjectKR_Sun::GetBiomeInfluenceStateAtLocation(const FVector& InLocation) const
{
	FProjectKR_SunInfluenceBiomeState LocalInfluence = CurrentBiomeInfluence;
 
	// 고도(Z)에 따른 온도 보정: 100m(10000cm) 상승마다 -0.65°C
	// [0,1] 스케일에서 1.0 = 약 40°C 범위라고 가정하면
	//   -0.65 / 40 = -0.016 per 100m
	const float AltitudeKm = InLocation.Z / 100000.0f;  // cm → km
	LocalInfluence.TemperatureOffset -= AltitudeKm * 0.016f * 10.0f;  // 1km당 -0.16
 
	// 고도가 높을수록 습도 감소 (기압 하강)
	LocalInfluence.HumidityOffset -= FMath::Clamp(AltitudeKm * 0.05f, 0.0f, 0.3f);
 
	return LocalInfluence;
}

void AProjectKR_Sun::ComputeSunState()
{
	// 1.	태양 적위 (Solar Declination)
	//		하지(AnnualCycle=0.25): +23.45°, 동지(0.75): -23.45°
	const float DeclinationDeg = -23.45f * FMath::Cos(2.0f * PI * (SeasonCycle + 0.0833f));
	const float LatRad = FMath::DegreesToRadians(Latitude);
	const float DecRad = FMath::DegreesToRadians(DeclinationDeg);
 
	// 2.	시간각 (정오 = 0, 자정 = ±180°)
	const float HourAngleDeg = (DayTime - 0.5f) * 360.0f;
	const float HARad = FMath::DegreesToRadians(HourAngleDeg);
	
	// 3.	고도각
	const float SinElev = FMath::Sin(LatRad) * FMath::Sin(DecRad) + FMath::Cos(LatRad) * FMath::Cos(DecRad) * FMath::Cos(HARad);
	const float ElevRad  = FMath::Asin(FMath::Clamp(SinElev, -1.0f, 1.0f));
	const float ElevDeg  = FMath::RadiansToDegrees(ElevRad);
	
	// 4. 방위각
	float AzimuthDeg = 180.0f;  // 기본값: 남쪽
	const float CosElev = FMath::Cos(ElevRad);
	if(CosElev > KINDA_SMALL_NUMBER)
	{
		const float CosAz = (FMath::Sin(DecRad) - FMath::Sin(LatRad) * SinElev) / (CosElev * FMath::Cos(LatRad));
		AzimuthDeg = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(CosAz, -1.0f, 1.0f)));
		if(DayTime > 0.5f)
		{
			// 오후: 서쪽으로 이동
			AzimuthDeg = 360.0f - AzimuthDeg;
		}
	}
	
	// 5. 낮 길이 계산 (일출~일몰 각도 비율)
	// cos(HourAngle_sunrise) = -tan(Lat)*tan(Dec)
	const float CosHASunrise = -FMath::Tan(LatRad) * FMath::Tan(DecRad);
	float DaylightFraction = 0.5f;  // 기본: 12시간
	if(FMath::Abs(CosHASunrise) <= 1.0f)
	{
		const float HASunriseDeg = FMath::RadiansToDegrees(FMath::Acos(CosHASunrise));
		DaylightFraction = HASunriseDeg / 180.0f;  // 0~1
	}
	else
	{
		// 극야(CosHA > 1) or 백야(CosHA < -1)
		DaylightFraction = (CosHASunrise > 1.0f) ? 0.0f : 1.0f;
	}
	
	// 6. 대기 감쇠 (공기층 두께에 따른 강도 감소)
	// Kasten-Young 공식 근사: airmass = 1 / (sin(Elev) + 0.50572*(Elev+6.07995)^-1.6364)
	float NormalizedIntensity = 0.0f;
	if(ElevDeg > 0.0f)
	{
		const float AirMass = 1.0f / (SinElev + 0.50572f * FMath::Pow(ElevDeg + 6.07995f, -1.6364f));
		// 대기 투과율 (지수 감쇠): T^AirMass, T = 0.7 (평균 대기 투명도)
		NormalizedIntensity = FMath::Pow(0.7f, AirMass) * SinElev;
		NormalizedIntensity = FMath::Clamp(NormalizedIntensity, 0.0f, 1.0f);
	}
	
	// 7. 색온도 계산 (고도각 기반)
	// 일출/일몰(낮은 고도): 2000~3000K (주황빨강)
	// 정오(높은 고도): 5800~6500K (흰색)
	float ColorTempK;
	if(ElevDeg <= 0.0f)
	{
		ColorTempK = 2000.0f;  // 야간: 달빛 (차가운 파랑)
	}
	else if(ElevDeg < 10.0f)
	{
		ColorTempK = FMath::Lerp(2200.0f, 4000.0f, ElevDeg / 10.0f);
	}
	else if(ElevDeg < 30.0f)
	{
		ColorTempK = FMath::Lerp(4000.0f, 5500.0f, (ElevDeg - 10.0f) / 20.0f);
	}
	else
	{
		ColorTempK = FMath::Lerp(5500.0f, 6200.0f, FMath::Min((ElevDeg - 30.0f) / 60.0f, 1.0f));
	}
	
	// 8. 계절 분류
	EProjectKR_SeasonType Season;
	if(SeasonCycle < 0.25f)			Season = EProjectKR_SeasonType::Spring;
	else if(SeasonCycle < 0.50f)	Season = EProjectKR_SeasonType::Summer;
	else if(SeasonCycle < 0.75f)	Season = EProjectKR_SeasonType::Autumn;
	else							Season = EProjectKR_SeasonType::Winter;
	
	// 9. FSunState 채우기
	CurrentSunState.ElevationAngle			= ElevDeg;
	CurrentSunState.Azimuth					= AzimuthDeg;
	CurrentSunState.NormalizedIntensity		= NormalizedIntensity;
	CurrentSunState.DayTime					= DayTime;
	CurrentSunState.SeasonCycle				= SeasonCycle;
	CurrentSunState.SeasonType				= Season;
	CurrentSunState.ColorKelvinTemperature	= ColorTempK;
	CurrentSunState.DayLightFraction		= DaylightFraction;
	
	// 10. DirectionalLight 방향 업데이트
	//     UE5 좌표계: X=앞, Y=오른쪽, Z=위
	//     방위각(북=0, 동=90) + 고도각 → Rotator
	const float PitchDeg = ElevDeg - 90.0f;  // DirectionalLight는 -Z 방향 기준
	const float YawDeg = AzimuthDeg - 180.0f;
	DirectionalLightComponent->SetWorldRotation(FRotator(PitchDeg, YawDeg, 0.0f));
}

