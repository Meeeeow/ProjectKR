// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SeedExt_Core/SubSystem/SeedExt_SubSystemBase.h"
#include "ProjectKR_CameraSubSystem.generated.h"

UCLASS()
class PROJECTKR_API UProjectKR_CameraSubSystem : public USeedExt_GameInstanceSubSystem
{
	GENERATED_BODY()
	

private:
	UPROPERTY() TObjectPtr<ACameraActor> CameraActor = nullptr;
	UPROPERTY() TObjectPtr<ACameraActor> CameraActor_External = nullptr;
	UPROPERTY() TObjectPtr<APlayerCameraManager> PlayerCameraManager = nullptr;

	TWeakObjectPtr<AActor> ActiveCameraActor = nullptr;

	UPROPERTY() TWeakObjectPtr<APawn> FollowPawn;
	float FollowPawnAddedHeight = 0.0f;
	FName FollowPawnBoneName = FName();
	UPROPERTY() TWeakObjectPtr<APawn> TargetPawn;
	float TargetPawnAddedHeight = 0.0f;
	FName TargetPawnBoneName = FName();
	
	virtual bool Tick_World(float InDeltaSeconds) override;

	void SpawnCameraActor();
	void DespawnCameraActor();
	
	void UpdatePlayerCameraManager();

public:
	FORCEINLINE ACameraActor* GetCameraActor(){ return (CameraActor_External!=nullptr) ? CameraActor_External : CameraActor; }
	void SetExternalCameraActor(ACameraActor* pInExternalCameraActor);
	FORCEINLINE APlayerCameraManager* GetPlayerCameraManager(){ return PlayerCameraManager; }
};
