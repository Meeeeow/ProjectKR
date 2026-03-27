// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	
};
