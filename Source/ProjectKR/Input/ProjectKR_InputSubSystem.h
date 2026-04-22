// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "SeedExt_Core/SubSystem/SeedExt_SubSystemBase.h"
#include "ProjectKR_InputSubSystem.generated.h"

#define ProjectKR_GetInputManager(World) FSeedExt_SubSystemCollector::_GetSubSystem<UProjectKR_InputSubSystem>(World)
#define ProjectKR_HasInputManager(World) FSeedExt_SubSystemCollector::_HasSubSystem<UProjectKR_InputSubSystem>(World)

USTRUCT(BlueprintType)
struct PROJECTKR_API FProjectKR_InputSource
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|InputConfig",meta=(AllowPrivateAccess))
	TObjectPtr<class UInputMappingContext> InputMappingContext;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|InputConfig")
	TMap<FName, class UInputAction*> InputAction_List;				// <Action Name, InputAction>
};

UCLASS(ClassGroup=(ProjectKR),Blueprintable)
class PROJECTKR_API UProjectKR_InputConfig : public UObject
{
	GENERATED_BODY()

public:
	const FProjectKR_InputSource* GetInputMappingSource(const FName& InContextName)
	{
		return InputActionSource_List.Find(InContextName);
	}

private:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="KR|InputConfig",meta=(AllowPrivateAccess))
	TMap<FName, FProjectKR_InputSource> InputActionSource_List;		// <ContextName, InputSource>
};

UCLASS()
class PROJECTKR_API UProjectKR_InputSubSystem : public USeedExt_LocalPlayerSubSystem
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Reset() override;
	void UpdatePlayerController();

	TObjectPtr<class APlayerController> GetPlayerController();
	void SetPlayerController(class APlayerController* InPlayerController);
	const FName& GetPlayingContextName() const { return PlayingContextName; }
	const FProjectKR_InputSource* GetPlayingInputConfigSource() const;

private:
	UPROPERTY(Transient) TWeakObjectPtr<class APlayerController> PlayerController = nullptr;
	UPROPERTY(Transient) TObjectPtr<UProjectKR_InputConfig> InputConfig = nullptr;

	FName PlayingContextName = NAME_None;

	void InitEnhancedInput();
	void ResetEnhancedInput();

protected:
	virtual UClass* GetSubSystemClass() override;
	virtual FString GetSubSystemName() override;
};
