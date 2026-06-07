// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "ProjectKR_AbilitySystemComponent.generated.h"

UCLASS(ClassGroup=(ProjectKR), meta=(BlueprintSpawnableComponent))
class PROJECTKR_API UProjectKR_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UProjectKR_AbilitySystemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InputTagPressed(const struct FGameplayTag& InInputTag);
	void InputTagReleased(const struct FGameplayTag& InInputTag);

	FGameplayAbilitySpecHandle GiveAbilityWithInputTag(TSubclassOf<class UGameplayAbility> InAbilityClass, const struct FGameplayTag& InInputTag, int32 InLevel = 1);
};
