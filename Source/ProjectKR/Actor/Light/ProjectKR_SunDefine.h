#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EProjectKR_SeasonType : uint8
{
	None = 0,
	Spring			/** 0.00 ~ 0.25 */,
	Summer			/** 0.25 ~ 0.50 */,
	Autumn			/** 0.50 ~ 0.75 */,
	Winter			/** 0.75 ~ 1.00 */,
	Num				UMETA(Hidden),
};

UENUM(BlueprintType)
enum class EProjectKR_LatitudeBandType : uint8
{
	None = 0,
	Polar,
	Subpolar,
	Temperate,
	SubTemperate,
	Tropical,
	Num				UMETA(Hidden),
};

/** ProjectKR_Sun이 실시간으로 계산하는 태양의 물리적 상태.
 * ElevationAngle은 수평선 기준의 각도 0 = 일출/일몰, 90 = 천정
 * Azimuth는 북쪽 기준의 시계방향 각도 0 = 북, 90 = 동, 180 = 남, 270 = 서
 */
USTRUCT(BlueprintType)
struct FProjectKR_SunState
{
	GENERATED_BODY()

	/** 태양 고도각 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Sun")
	float ElevationAngle = 45.0f;
	/** 태양 방위각 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Sun",meta=(ClampMin="0.0",ClampMax="360.0",UIMin="0.0",UIMax="360.0"))
	float Azimuth = 180.0f;
	/** 태양 강도 ( 정규화 ) */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Sun",meta=(ClampMin="0.0",ClampMax="1.0",UIMin="0.0",UIMax="1.0"))
	float NormalizedIntensity = 1.0f;
	/** 현재 시간 ( 정규화 ) */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Sun",meta=(ClampMin="0.0",ClampMax="1.0",UIMin="0.0",UIMax="1.0"))
	float DayTime = 0.5f;
	/** 연간 주기 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Sun",meta=(ClampMin="0.0",ClampMax="1.0",UIMin="0.0",UIMax="1.0"))
	float SeasonCycle = 0.25f;
	/** 현재 계절 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Sun")
	EProjectKR_SeasonType SeasonType = EProjectKR_SeasonType::None;
	/** 태양 색온도 ( Kelvin ) */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Sun")
	float ColorKelvinTemperature = 6000.f;
	/** 낮 시간 비율 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Sun",meta=(ClampMin="0.0",ClampMax="1.0",UIMin="0.0",UIMax="1.0"))
	float DayLightFraction = 0.6f;

	bool IsDayTime() const { return ElevationAngle > 0.0f; }
	/** Irradiance is the total amount of light incident on a surface. ( 복사조도는 단위 면적의 표면에 입사하는 방사 에너지[복사속, Radiant Flux]의 양을 나타내는 물리량 ) */
	float GetSolarIrrandianceFactor() const
	{
		return FMath::Max(0.0f, FMath::Sin(FMath::DegreesToRadians(ElevationAngle)));
	}
	EProjectKR_LatitudeBandType GetLatitudeBandType() const
	{
		if(ElevationAngle < 20.0f)	return EProjectKR_LatitudeBandType::Polar;
		if(ElevationAngle < 40.0f)	return EProjectKR_LatitudeBandType::Subpolar;
		if(ElevationAngle < 60.0f)	return EProjectKR_LatitudeBandType::Temperate;
		if(ElevationAngle < 75.0f)	return EProjectKR_LatitudeBandType::SubTemperate;
		
		return EProjectKR_LatitudeBandType::Tropical;
	}
};

/** FProjectKR_SunState에서 파생된 바이옴을 결정하기 위한 파라미터 */
USTRUCT(BlueprintType)
struct FProjectKR_SunInfluenceBiomeState
{
	GENERATED_BODY()

	//--------------------------------------------------// Temperature
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Influence")
	float TemperatureOffset = 0.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Influence")
	float TemperatureMultiplier = 1.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Influence")
	float DiurnalAplitude = 0.1f;

	//--------------------------------------------------// Humidity
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Influence")
	float HumidityOffset = 0.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Influence")
	float HumidityMultiplier = 1.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Influence")
	float SeasonalPrecipitation = 1.0f;

	//--------------------------------------------------// Influence
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Influence")
	EProjectKR_LatitudeBandType LatitudeBandType = EProjectKR_LatitudeBandType::None;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Influence")
	float SeasonalWarmth = 0.5f;
	/** 대기 투명도 (안개/구름 효과, 0=불투명 ~ 1=맑음) */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Influence")
	float AtmosphericClarity = 1.0f;

	float ApplyToTemperature(float InNoiseTemperature) const
	{
		const float Result = InNoiseTemperature * TemperatureMultiplier + TemperatureOffset;
		return FMath::Clamp(Result, 0.0f, 1.0f);
	}
	float ApplyToHumidity(float InNoiseHumidity) const
	{
		const float Result = InNoiseHumidity * HumidityMultiplier + HumidityOffset;
		return FMath::Clamp(Result, 0.0f, 1.0f);
	}
};

USTRUCT(BlueprintType)
struct FProjectKR_SunAtomsphereParams
{
	GENERATED_BODY()

	/** 태양광 색상 (고도각에 따라 주황~흰색) */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Atmosphere")
	FLinearColor SunLightColor = FLinearColor::White;
	/** 태양광 조도 (Lux) */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Atmosphere")
	float SunLightIntensity = 100000.0f;
	/** 앰비언트 스카이 색상 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Atmosphere")
	FLinearColor SkyColor = FLinearColor(0.2f, 0.4f, 1.0f);
	/** 안개 밀도 (새벽/황혼에 최대) */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Atmosphere")
	float FogDensity = 0.02f;
	/** 산란 강도 (일출/일몰 시 레일리 산란 강조) */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Atmosphere")
	float RayleighScattering = 1.0f;
	/** 하늘 조명 강도 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Atmosphere")
	float SkyLightIntensity = 1.0f;
};