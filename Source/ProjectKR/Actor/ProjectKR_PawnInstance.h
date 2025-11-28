// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "SeedExt_Core/SeedExt_Define.h"

#include "ProjectKR_PawnInstance.generated.h"

UCLASS()
class PROJECTKR_API AProjectKR_PawnInstance : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	explicit AProjectKR_PawnInstance(const FObjectInitializer& ObjectInitializer=FObjectInitializer::Get());

	//TObjectPtr<class UGameKR_MovementComponent> GetMovementComponentOverride();
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="SeedExt|Pawn|Component")
	TObjectPtr<class USceneComponent> RootSceneComponent;
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="SeedExt|Pawn|Component")
	TObjectPtr<class UArrowComponent> ArrowComponent;
#endif
	//UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="SeedExt|Pawn|Component")
	//TObjectPtr<class USeedExt_MovementComponent> MovementComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="SeedExt|Pawn|Component")
	TObjectPtr<class UShapeComponent> ShapeComponent;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Pawn|Component")
	TObjectPtr<class USkeletalMeshComponent> SkeletalMeshComponent;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Pawn")
	FTopLevelAssetPath BPPath;

	//UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SeedExt|Pawn")
	//ESeedExt_PawnType PawnType = ESeedExt_PawnType::None;

	ESeedExt_PlayingStateType PawnPlayingState = ESeedExt_PlayingStateType::None;
	ESeedExt_LoadingStateType PawnLoadingState = ESeedExt_LoadingStateType::None;

	virtual void Init();
	virtual void ClearAll();
	virtual void Reset() override;

	//~ Begin UObject Interface.
	virtual void Serialize(FArchive& Ar) override;
	//~ End UObject Interface

	//~ Begin AActor Interface.
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	//~ End AActor Interface

	UFUNCTION(BlueprintCallable) void SetPawnBP(class UBlueprintGeneratedClass* InBlueprintGeneratedClass);

	virtual void PossessedBy(AController* NewController) override;
};
