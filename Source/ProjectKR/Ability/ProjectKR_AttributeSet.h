// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "SeedExt_Core/SeedExt_Define.h"

#include "ProjectKR_AttributeSet.generated.h"

#define PROJECTKR_ATTRIBUTE_DEFINE(ClassName,PropertyName)																					\
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName,PropertyName)																				\
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)																							\
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)																							\
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS(ClassGroup="ProjectKR")
class PROJECTKR_API UProjectKR_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UFUNCTION() void OnRep_Health(const FGameplayAttributeData& InOldValue);
	UFUNCTION() void OnRep_MaxHealth(const FGameplayAttributeData& InOldValue);
	UFUNCTION() void OnRep_Stamina(const FGameplayAttributeData& InOldValue);
	UFUNCTION() void OnRep_MaxStamina(const FGameplayAttributeData& InOldValue);
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Health,Category="KR|Attribute")
	FGameplayAttributeData Health;
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_MaxHealth,Category="KR|Attribute")
	FGameplayAttributeData MaxHealth;
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Stamina,Category="KR|Attribute")
	FGameplayAttributeData Stamina;
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_MaxStamina,Category="KR|Attribute")
	FGameplayAttributeData MaxStamina;

public:
	PROJECTKR_ATTRIBUTE_DEFINE(UProjectKR_AttributeSet, Health)
	PROJECTKR_ATTRIBUTE_DEFINE(UProjectKR_AttributeSet, MaxHealth)
	PROJECTKR_ATTRIBUTE_DEFINE(UProjectKR_AttributeSet, Stamina)
	PROJECTKR_ATTRIBUTE_DEFINE(UProjectKR_AttributeSet, MaxStamina)
};
