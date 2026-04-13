// Fill out your copyright notice in the Description page of Project Settings.

#include "SeedExt_Sun.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"

ASeedExt_Sun::ASeedExt_Sun()
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
void ASeedExt_Sun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASeedExt_Sun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#if WITH_EDITOR
void ASeedExt_Sun::PostEditChangeProperty(FPropertyChangedEvent& InEvent)
{
	Super::PostEditChangeProperty(InEvent);
	ComputeSunState();
	ComputeBiomeInfluence();
	ApplyAtmosphereParams(ComputeAtmosphereParams());
}
#endif

void ASeedExt_Sun::SetDayTime(float InNewDayTime)
{
	DayTime = FMath::Clamp(InNewDayTime, 0.f, 1.f);
	ComputeSunState();
	ComputeBiomeInfluence();
	ApplyAtmosphereParams(ComputeAtmosphereParams());
	OnBiomeInfluenceChangedDelegate.Broadcast(CurrentBiomeInfluence);
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
	ComputeBiomeInfluence();
	ApplyAtmosphereParams(ComputeAtmosphereParams());
	OnBiomeInfluenceChangedDelegate.Broadcast(CurrentBiomeInfluence);
	OnSeasonChangedDelegate.Broadcast(CurrentSunState.SeasonType, PreviousSeasonType);
}

FSeedExt_SunInfluenceBiomeState ASeedExt_Sun::GetBiomeInfluenceStateAtLocation(const FVector& InLocation) const
{
	FSeedExt_SunInfluenceBiomeState LocalState = CurrentBiomeInfluence;
	{
		const float AltKm = InLocation.Z / 100000.f;
		LocalState.TemperatureOffset -= AltKm * 0.16f;
		LocalState.HumidityOffset -= FMath::Clamp(AltKm * 0.05f, 0.f, 0.3f);
	}
	
	return LocalState;
}

void ASeedExt_Sun::ComputeSunState()
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
	const float ElevRad = FMath::Asin(FMath::Clamp(SinElev, -1.0f, 1.0f));
	const float ElevDeg = FMath::RadiansToDegrees(ElevRad);
	
	// 4.	방위각
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
	ESeedExt_SeasonType Season;
	if(SeasonCycle < 0.25f)			Season = ESeedExt_SeasonType::Spring;
	else if(SeasonCycle < 0.50f)	Season = ESeedExt_SeasonType::Summer;
	else if(SeasonCycle < 0.75f)	Season = ESeedExt_SeasonType::Autumn;
	else							Season = ESeedExt_SeasonType::Winter;
	
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
void ASeedExt_Sun::ComputeBiomeInfluence()
{
	
}
FSeedExt_SunAtomsphereParams ASeedExt_Sun::ComputeAtmosphereParams() const
{
	const FSeedExt_SunState& SunState = CurrentSunState;
	FSeedExt_SunAtomsphereParams SunParams;

	if(SunState.ElevationAngle <= 0.0f)
	{
		SunParams.SunLightColor = FLinearColor(0.05f, 0.07f, 0.15f);
	}
	else if(SunState.ElevationAngle < 15.f)
	{
		SunParams.SunLightColor = FLinearColor::LerpUsingHSV(SunriseColor, NoonColor, SunState.ElevationAngle / 15.f);
	}
	else
	{
		SunParams.SunLightColor = FLinearColor::LerpUsingHSV(NoonColor * 0.9f, NoonColor, FMath::Clamp((SunState.ElevationAngle - 15.f) / 75.f, 0.f, 1.f));
	}

	if(SunState.ElevationAngle <= 0.0f)
	{
		SunParams.SkyColor = FLinearColor(0.005f, 0.005f, 0.02f);
	}
	else if(SunState.ElevationAngle < 10.0f)
	{
		SunParams.SkyColor = FLinearColor::LerpUsingHSV(FLinearColor(0.3f, 0.1f, 0.f), FLinearColor(0.05f, 0.15f, 0.5f), SunState.ElevationAngle / 10.f);
	}
	else
	{
		SunParams.SkyColor = FLinearColor(0.05f, 0.15f, 0.5f);
	}

	SunParams.FogDensity = FMath::Lerp(0.08f, 0.015f, FMath::Clamp(SunState.ElevationAngle / 30.f, 0.f, 1.f));
	SunParams.SkyLightIntensity = FMath::Clamp(SunState.NormalizedIntensity * 2.0f, 0.05f, 2.f);

	return SunParams;
}
void ASeedExt_Sun::ApplyAtmosphereParams(const FSeedExt_SunAtomsphereParams& InParams)
{
	if(DirectionalLightComponent != nullptr)
	{
		DirectionalLightComponent->SetLightColor(InParams.SunLightColor);
		DirectionalLightComponent->SetIntensity(InParams.SunLightIntensity);
	}

	if(SkyLightComponent != nullptr)
	{
		SkyLightComponent->SetIntensity(InParams.SkyLightIntensity);
		if(SkyLightComponent->bRealTimeCapture == false)
		{
			SkyLightComponent->RecaptureSky();
		}
	}

	if(SkyAtmosphereComponent != nullptr)
	{
		SkyAtmosphereComponent->SetRayleighScatteringScale(InParams.RayleighScattering);
	}

	if(ExponentialHeightFogComponent != nullptr)
	{
		ExponentialHeightFogComponent->SetFogDensity(InParams.FogDensity);
		ExponentialHeightFogComponent->SetFogInscatteringColor(InParams.SkyColor);
	}
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

