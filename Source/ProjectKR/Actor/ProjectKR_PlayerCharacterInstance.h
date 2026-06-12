// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"

#include "ProjectKR/Input/ProjectKR_InputReceiveInterface.h"

#include "SeedExt_Core/Actor/SeedExt_CharacterInstance.h"

#include "ProjectKR_PlayerCharacterInstance.generated.h"

UCLASS(Blueprintable)
class PROJECTKR_API AProjectKR_PlayerCharacterInstance : public ASeedExt_CharacterInstance, public IAbilitySystemInterface, public IProjectKR_InputReceiveInterface
{
	GENERATED_BODY()

public:
	AProjectKR_PlayerCharacterInstance();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void OnRep_PlayerState() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="KR|Components")
	TObjectPtr<class UProjectKR_SpringArmComponent> SpringArmComponent = nullptr;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="KR|Components")
	TObjectPtr<class UCameraComponent> CameraComponent = nullptr;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="KR|Components")
	TObjectPtr<class UProjectKR_AbilitySystemComponent> AbilitySystemComponent = nullptr;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="KR|Components")
	TObjectPtr<class UProjectKR_AttributeSet> AttributeSet = nullptr;

	UPROPERTY(EditDefaultsOnly,Category="KR|Components")
	TMap<FGameplayTag, TSubclassOf<class UGameplayAbility>> GameplayAbility_List; 
public:
	virtual void HandleMoveInput(const FVector2D& InAxis) override;
	virtual void HandleLookInput(const FVector2D& InAxis) override;
	virtual void HandleJumpStart() override;
	virtual void HandleJumpStop() override;

};
