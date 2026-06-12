// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ProjectKR_PlayerController.generated.h"

UCLASS(ClassGroup=(ProjectKR))
class PROJECTKR_API AProjectKR_PlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	void OnCharacterSpawnComplete(struct FSeedExt_CharacterHandle InCharacterHandle, FSoftObjectPath InCharacterPath);
	void OnCharacterSpawnFail(struct FSeedExt_CharacterHandle InCharacterHandle, FSoftObjectPath InCharacterPath);
	
public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	class AProjectKR_PlayerCharacterInstance* GetPlayerCharacterInstance();

	UFUNCTION(Client,Reliable)
	void ClientRPC_OnPlayerReadyComplete(const FSoftObjectPath& InCharacterPath);
	UFUNCTION(Server,Reliable)
	void Server_RequestSpawn(const FSoftObjectPath& InCharacterPath);

private:
	void BindInputAction(class UEnhancedInputComponent* InEnhancedInputComponent);
	void UnbindInputAction();

	void Input_Move(const struct FInputActionValue& InInputActionValue);
	void Input_Look(const struct FInputActionValue& InInputActionValue);
	void Input_JumpStart(const struct FInputActionValue& InInputActionValue);
	void Input_JumpComplete(const struct FInputActionValue& InInputActionValue);

	void Input_AbilityInputTagPressed(struct FGameplayTag InGameplayTag);
	void Input_AbilityInputTagReleased(struct FGameplayTag InGameplayTag);
};
