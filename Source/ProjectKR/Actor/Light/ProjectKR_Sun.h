// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ProjectKR_SunDefine.h"

#include "ProjectKR_Sun.generated.h"

UCLASS()
class PROJECTKR_API AProjectKR_Sun : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectKR_Sun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	/** 태양 방향광 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Components")
	TObjectPtr<class UDirectionalLightComponent> DirectionalLightComponent;
	/** 빛 색상 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Components")
	TObjectPtr<class USkyLightComponent> SkyLightComponent;
	/** 대기 산란 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Components")
	TObjectPtr<class USkyAtmosphereComponent> SkyAtmosphereComponent;
	/** 높이 기반 안개 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Components")
	TObjectPtr<class UExponentialHeightFogComponent> ExponentialHeightFogComponent;
	/** 태양 메시 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Components")
	TObjectPtr<class UStaticMeshComponent> StaticMeshComponent;

	/** 위도 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Info",meta=(ClampMin="-90.0",ClampMax="90.0",UIMin="-90.0",UIMax="90.0"))
	float Latitude = 35.f;
	/** 시간 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Info",meta=(ClampMin="0",ClampMax="1",UIMin="0",UIMax="1"))
	float DayTime = 0.5f;
	/** 연간 주기 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Info",meta=(ClampMin="0",ClampMax="1",UIMin="0",UIMax="1"))
	float SeasonCycle = 0.25f;
	/** 시간 진행 속도 배율 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Info",meta=(ClampMin="0"))
	float DayTimeScale = 600.f;
	/** 시간 진행 속도 배율 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Info",meta=(ClampMin="0"))
	float SeasonCycleScale = 0.0f;

	/** 최대 태양광 조도 (정오 기준 Lux) */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Info",meta=(ClampMin="0"))
	float MaxSunlightLux = 120000.0f;
	/** 최대 야간 달빛 강도 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Info",meta=(ClampMin="0"))
	float MoonlightIntensity = 0.1f;
	/** 일출/일몰 시 대기 색상 (레일리 산란 색) */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Info")
	FLinearColor SunriseColor = FLinearColor(1.0f, 0.4f, 0.1f);
	/** 정오 태양광 색상 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Info")
	FLinearColor NoonColor = FLinearColor(1.0f, 0.98f, 0.9f);
 
	/** 현재 태양 물리 상태 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Info")
	FProjectKR_SunState CurrentSunState;
	/** 현재 바이옴 영향 파라미터 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Info")
	FProjectKR_SunInfluenceBiomeState CurrentBiomeInfluence;

public:
	UFUNCTION(BlueprintImplementableEvent,Category="KR|Events")
	void OnBiomeInfluenceChanged(const FProjectKR_SunInfluenceBiomeState& InBiomeInfluence);

	UFUNCTION(BlueprintImplementableEvent,Category="KR|Events")
	void OnSeasonChanged(EProjectKR_SeasonType InNewSeason, EProjectKR_SeasonType InOldSeason);

	UFUNCTION(BlueprintImplementableEvent,Category="KR|Events")
	void OnSunrise(bool InSunrise);

	UFUNCTION(BlueprintCallable,Category="KR|Events")
	void SetDayTime(float InNewDayTime);

	UFUNCTION(BlueprintCallable,Category="KR|Events")
	void SetSeasonType(EProjectKR_SeasonType InNewSeasonType);

	UFUNCTION(BlueprintPure,Category="KR|Events")
	FProjectKR_SunInfluenceBiomeState GetBiomeInfluenceState() const { return CurrentBiomeInfluence; }

	UFUNCTION(BlueprintPure,Category="KR|Events")
	FProjectKR_SunState GetSunState() const { return CurrentSunState; }

	UFUNCTION(BlueprintPure,Category="KR|Events")
	FProjectKR_SunInfluenceBiomeState GetBiomeInfluenceStateAtLocation(const FVector& InLocation) const;

private:
	void ComputeSunState();
};
