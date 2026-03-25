#pragma once

#include "CoreMinimal.h"

#include "Engine/DirectionalLight.h"

#include "ProjectKR_LightInterface.h"
#include "ProjectKR/Actor/Interface/ProjectKR_ExportPropertyInterface.h"

#include "ProjectKR_DirectionalLight.generated.h"

UCLASS()
class PROJECTKR_API AProjectKR_DirectionalLight : public ADirectionalLight, public IProjectKR_ExportPropertyInterface, public IProjectKR_LightInterface
{
	GENERATED_BODY()

public:
	AProjectKR_DirectionalLight();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void UpdateLightVisual() override;

protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Climate")
	float SunInfluenceWeight = 1.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Climate")
	FLinearColor NoonColor = FLinearColor(1.0f, 1.0f, 0.9f);
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|Climate")
	FLinearColor SunsetColor = FLinearColor(1.0f, 0.4f, 0.2f);
};
