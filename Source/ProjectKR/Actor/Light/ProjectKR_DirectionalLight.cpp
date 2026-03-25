// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_DirectionalLight.h"

#include "Components/LightComponent.h"

AProjectKR_DirectionalLight::AProjectKR_DirectionalLight()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AProjectKR_DirectionalLight::BeginPlay()
{
	Super::BeginPlay();

	UpdateLightVisual();
}

void AProjectKR_DirectionalLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
#if WITH_EDITOR
void AProjectKR_DirectionalLight::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	ExportProperty();
	UpdateLightVisual();
}
#endif
void AProjectKR_DirectionalLight::UpdateLightVisual()
{
	if(ULightComponent* SkyDirectionalLightComponent = GetLightComponent())
	{
		float SunHeight = -GetActorForwardVector().Z;
		SunHeight = FMath::Max(0.0f, SunHeight);

		FLinearColor FinalColor = FLinearColor::LerpUsingHSV(SunsetColor, NoonColor, SunHeight);

		SkyDirectionalLightComponent->SetLightColor(FinalColor);
		SkyDirectionalLightComponent->SetIntensity(SunHeight * 10.0f);
	}
}

