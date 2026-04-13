// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SeedExt_SunDefine.h"

#include "GameFramework/Actor.h"
#include "SeedExt_Sun.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSeedExt_SunBiomeInfluenceChangedDelegate, const FSeedExt_SunInfluenceBiomeState&, InNewInfluence);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSeedExt_SeasonChangedDelegate, ESeedExt_SeasonType, InNewSeason, ESeedExt_SeasonType, InOldSeason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSeedExt_SunriseDelegate, bool, bIsSunrise);

UCLASS()
class SEEDEXT_CORE_API ASeedExt_Sun : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASeedExt_Sun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& InEvent);
#endif


protected:
	/** 태양 방향광 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Components")
	TObjectPtr<class UDirectionalLightComponent> DirectionalLightComponent;
	/** 빛 색상 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Components")
	TObjectPtr<class USkyLightComponent> SkyLightComponent;
	/** 대기 산란 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Components")
	TObjectPtr<class USkyAtmosphereComponent> SkyAtmosphereComponent;
	/** 높이 기반 안개 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Components")
	TObjectPtr<class UExponentialHeightFogComponent> ExponentialHeightFogComponent;
	/** 태양 메시 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Components")
	TObjectPtr<class UStaticMeshComponent> StaticMeshComponent;

	/** 위도 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Info",meta=(ClampMin="-90.0",ClampMax="90.0",UIMin="-90.0",UIMax="90.0"))
	float Latitude = 35.f;
	/** 시간 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Info",meta=(ClampMin="0",ClampMax="1",UIMin="0",UIMax="1"))
	float DayTime = 0.5f;
	/** 연간 주기 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Info",meta=(ClampMin="0",ClampMax="1",UIMin="0",UIMax="1"))
	float SeasonCycle = 0.25f;
	/** 시간 진행 속도 배율 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Info",meta=(ClampMin="0"))
	float DayTimeScale = 600.f;
	/** 시간 진행 속도 배율 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Info",meta=(ClampMin="0"))
	float SeasonCycleScale = 0.0f;

	/** 최대 태양광 조도 (정오 기준 Lux) */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Info",meta=(ClampMin="0"))
	float MaxSunlightLux = 120000.0f;
	/** 최대 야간 달빛 강도 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Info",meta=(ClampMin="0"))
	float MoonlightIntensity = 0.1f;
	/** 일출/일몰 시 대기 색상 (레일리 산란 색) */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Info")
	FLinearColor SunriseColor = FLinearColor(1.0f, 0.4f, 0.1f);
	/** 정오 태양광 색상 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Info")
	FLinearColor NoonColor = FLinearColor(1.0f, 0.98f, 0.9f);
 
	/** 현재 태양 물리 상태 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Info")
	FSeedExt_SunState CurrentSunState;
	/** 현재 바이옴 영향 파라미터 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Info")
	FSeedExt_SunInfluenceBiomeState CurrentBiomeInfluence;

public:
	UPROPERTY(BlueprintAssignable,Category="SeedExt|Events")
	FSeedExt_SunBiomeInfluenceChangedDelegate OnBiomeInfluenceChangedDelegate;
	UPROPERTY(BlueprintAssignable,Category="SeedExt|Events")
	FSeedExt_SeasonChangedDelegate OnSeasonChangedDelegate;
	UPROPERTY(BlueprintAssignable,Category="SeedExt|Events")
	FSeedExt_SunriseDelegate OnSunriseDelegate;

	UFUNCTION(BlueprintCallable,Category="SeedExt|Events")
	void SetDayTime(float InNewDayTime);
	UFUNCTION(BlueprintCallable,Category="SeedExt|Events")
	void SetSeasonType(ESeedExt_SeasonType InNewSeasonType);

	UFUNCTION(BlueprintPure,Category="SeedExt|Events")
	FSeedExt_SunInfluenceBiomeState GetBiomeInfluenceState() const { return CurrentBiomeInfluence; }

	UFUNCTION(BlueprintPure,Category="SeedExt|Events")
	FSeedExt_SunState GetSunState() const { return CurrentSunState; }

	UFUNCTION(BlueprintPure,Category="SeedExt|Events")
	FSeedExt_SunInfluenceBiomeState GetBiomeInfluenceStateAtLocation(const FVector& InLocation) const;

private:
	void ComputeSunState();
	void ComputeBiomeInfluence();
	FSeedExt_SunAtomsphereParams ComputeAtmosphereParams() const;
	void ApplyAtmosphereParams(const FSeedExt_SunAtomsphereParams& InParams);

	static FLinearColor ColorTemperatureToLinear(float InTemperature);

	ESeedExt_SeasonType PreviousSeasonType = ESeedExt_SeasonType::None;
	bool bWasDayTime = true;
	float BiomeInfluenceAccum = 0.0f;

	static constexpr float BiomeUpdateInterval = 1.f;
};
