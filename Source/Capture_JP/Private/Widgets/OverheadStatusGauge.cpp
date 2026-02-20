// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/OverheadStatusGauge.h"

#include "AbilitySystemComponent.h"
#include "Ability/CAttributeSet.h"

#include "Widgets/ValueGauge.h"

void UOverheadStatusGauge::SetOwningAbilitySystemComponent(UAbilitySystemComponent* AbilitySystemComponent)
{
	HealthBar->BindToGameplayAttribute(AbilitySystemComponent, UCAttributeSet::GetHealthAttribute(), UCAttributeSet::GetMaxHealthAttribute());
	ManaBar->BindToGameplayAttribute(AbilitySystemComponent, UCAttributeSet::GetManaAttribute(), UCAttributeSet::GetMaxManaAttribute());
}
