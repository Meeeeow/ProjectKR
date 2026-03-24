// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectKR_LightInterface.h"

#include "Engine/SkyLight.h"

#include "ProjectKR/Actor/Interface/ProjectKR_ExportPropertyInterface.h"

#include "ProjectKR_SkyLight.generated.h"

UCLASS()
class PROJECTKR_API AProjectKR_SkyLight : public ASkyLight, public IProjectKR_ExportPropertyInterface, public IProjectKR_LightInterface
{
	GENERATED_BODY()

public:
	AProjectKR_SkyLight();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void UpdateLightVisual() override;

protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Climate",meta=(ClampMin=-20,ClampMax=50,UIMin=-20,UIMax=50))
	float Temperature = 20.f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Climate",meta=(ClampMin=0,ClampMax=100,UIMin=0,UIMax=100))
	float Humidity = 50.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Climate")
	FLinearColor WarmWeather = FLinearColor(1.0f, 0.8f, 0.6f);
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Climate")
	FLinearColor ColdWeather = FLinearColor(0.6f, 0.8f, 1.0f);
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Climate")
	FLinearColor HumidityWeatherWeight = FLinearColor(0.3f, 0.3f, 0.2f);
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Climate",meta=(ClampMin=0,ClampMax=6,UIMin=0,UIMax=6))
	float MinimumIntensity = 0.5f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Climate",meta=(ClampMin=0,ClampMax=6,UIMin=0,UIMax=6))
	float MaximumIntensity = 3.5f;
	
public:
	float GetNormalizeTemperature() const { return (Temperature+20.f) / 70.f; }
	float GetNormalizeHumidity() const { return Humidity / 100.f; }
};
