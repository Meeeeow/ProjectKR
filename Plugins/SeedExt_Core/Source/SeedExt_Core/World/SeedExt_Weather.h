// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SeedExt_Weather.generated.h"

UCLASS()
class SEEDEXT_CORE_API ASeedExt_Weather : public AActor
{
	GENERATED_BODY()

public:
	ASeedExt_Weather();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	/** 날씨에 따른 파티클 컴포넌트 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Components")
	TObjectPtr<class UNiagaraComponent> WeatherNiagaraComponent = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Components")
	TObjectPtr<class UNiagaraSystem> WeatherNiagaraSystem = nullptr;
};
