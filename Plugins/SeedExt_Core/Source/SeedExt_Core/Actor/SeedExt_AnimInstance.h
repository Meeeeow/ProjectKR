// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SeedExt_AnimInstance.generated.h"

UENUM(BlueprintType)
enum class ESeedExt_DirectionAnimType : uint8
{
	None = 0,
	Forward,
	Backward,
	Left,
	Right,
	Num					UMETA(Hidden),
};

USTRUCT(BlueprintType)
struct SEEDEXT_CORE_API FSeedExt_DirectionAnimLocomotionInfo
{
	GENERATED_BODY()

	friend class USeedExt_AnimInstance;

public:
	void SetAnimation(class UAnimSequence* InAnimation, ESeedExt_DirectionAnimType InDirectionAnimType);
	TObjectPtr<class UAnimSequence> GetAnimation(ESeedExt_DirectionAnimType InDirectionAnimType);

protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="SeedExt|Locomotion")
	TMap<ESeedExt_DirectionAnimType, TObjectPtr<class UAnimSequence>> Animation_List;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="SeedExt|Locomotion")
	TObjectPtr<class UBlendSpace> Anim_BlendSpace = nullptr;
};

UCLASS(ClassGroup=(SeedExt))
class SEEDEXT_CORE_API USeedExt_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	friend class ASeedExt_CharacterInstance;

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="SeedExt|Locomotion")
	TObjectPtr<class UAnimSequence> Anim_Idle = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="SeedExt|Locomotion")
	FSeedExt_DirectionAnimLocomotionInfo Anim_Walk;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="SeedExt|Locomotion")
	FSeedExt_DirectionAnimLocomotionInfo Anim_Run;
};
