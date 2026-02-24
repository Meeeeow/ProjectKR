// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "SeedExt_Core/SubSystem/SeedExt_SubSystemBase.h"
#include "ProjectKR_InputSubSystem.generated.h"

#define ProjectKR_GetInputManager(World) FSeedExt_SubSystemCollector::_GetSubSystem<UProjectKR_InputSubSystem>(World)
#define ProjectKR_HasInputManager(World) FSeedExt_SubSystemCollector::_HasSubSystem<UProjectKR_InputSubSystem>(World)

UCLASS()
class PROJECTKR_API UProjectKR_InputSubSystem : public USeedExt_GameInstanceSubSystem
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Reset() override;
	void UpdatePlayerController();

	TObjectPtr<class APlayerController> GetPlayerController();

private:
	UPROPERTY() TObjectPtr<class APlayerController> PlayerController = nullptr;
	UPROPERTY() TObjectPtr<class UInputMappingContext> InputMappingContext = nullptr;
	UPROPERTY() TObjectPtr<class UEnhancedPlayerInput> PlayerInput = nullptr;
	UPROPERTY() TObjectPtr<class UEnhancedInputUserSettings> PlayerInputUserSettings = nullptr;

	void InitEnhancedInput();
	void ResetEnhancedInput();
};
