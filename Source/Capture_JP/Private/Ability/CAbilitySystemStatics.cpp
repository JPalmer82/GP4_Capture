// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/CAbilitySystemStatics.h"
#include "Abilities/GameplayAbility.h"
#include "CAbilitySystemStatics.h"

float UCAbilitySystemStatics::GetStaticCooldownDuration(const UGameplayAbility* Ability)
{
	UGameplayEffect* CooldownEffect = Ability->GetCooldownGameplayEffect();
	if (!CooldownEffect)
	{
		return 0.0f;
	}

	float CooldownDuration = 0.0f;
	CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1, CooldownDuration);
	return CooldownDuration;
}

float UCAbilitySystemStatics::GetStaticCost(const UGameplayAbility* Ability)
{
	UGameplayEffect* CostEffect = Ability->GetCostGameplayEffect();
	if (!CostEffect)
	{
		return 0.0f;
	}

	if (CostEffect->Modifiers.Num() == 0)
	{
		return 0;
	}

	float Cost = 0.0f;
	CostEffect->Modifiers[0].ModifierMagnitude.GetStaticMagnitudeIfPossible(1, Cost);
	return FMath::Abs(Cost);
}
