// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_AttributeSet.h"

#include "Net/UnrealNetwork.h"

void UProjectKR_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	for(TFieldIterator<FProperty> Iter(GetClass(), EFieldIterationFlags::None); Iter; ++Iter)
	{
		FProperty* Property = *Iter;
		if(Property->HasAnyPropertyFlags(EPropertyFlags::CPF_Net) == false)
			continue;

		FDoRepLifetimeParams LifetimeParams;
		LifetimeParams.Condition = ELifetimeCondition::COND_None;
		LifetimeParams.RepNotifyCondition = Property->HasAnyPropertyFlags(CPF_RepNotify)==true ? ELifetimeRepNotifyCondition::REPNOTIFY_Always : ELifetimeRepNotifyCondition::REPNOTIFY_OnChanged;

		RegisterReplicatedLifetimeProperty(Property, OutLifetimeProps, LifetimeParams);
	}
}

void UProjectKR_AttributeSet::OnRep_Health(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UProjectKR_AttributeSet, Health, InOldValue);
}
void UProjectKR_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UProjectKR_AttributeSet, MaxHealth, InOldValue);
}
void UProjectKR_AttributeSet::OnRep_Stamina(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UProjectKR_AttributeSet, Stamina, InOldValue);
}
void UProjectKR_AttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UProjectKR_AttributeSet, MaxStamina, InOldValue);
}
