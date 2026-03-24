// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_SkyLight.h"

#include "Components/SkyLightComponent.h"

AProjectKR_SkyLight::AProjectKR_SkyLight()
{
	PrimaryActorTick.bCanEverTick = true;

	if(USkyLightComponent* SkyLightComponent = GetLightComponent())
	{
		SkyLightComponent->bRealTimeCapture = true;
	}
}

void AProjectKR_SkyLight::BeginPlay()
{
	Super::BeginPlay();

	UpdateLightVisual();
}

void AProjectKR_SkyLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AProjectKR_SkyLight::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	ExportProperty();
	UpdateLightVisual();
}

void AProjectKR_SkyLight::UpdateLightVisual()
{
	if(USkyLightComponent* SkyLightComponent = GetLightComponent())
	{
		const float Alpha = GetNormalizeTemperature();
		FLinearColor FinalColor = FLinearColor::LerpUsingHSV(ColdWeather, WarmWeather, Alpha);

		FinalColor = FMath::Lerp(FinalColor, HumidityWeatherWeight, GetNormalizeHumidity());

		SkyLightComponent->SetLightColor(FinalColor);
		SkyLightComponent->SetIntensity(FMath::Lerp(MinimumIntensity, MaximumIntensity, Alpha));
	}
}

