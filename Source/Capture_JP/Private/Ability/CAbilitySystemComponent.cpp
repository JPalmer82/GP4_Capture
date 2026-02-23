// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/CAbilitySystemComponent.h"
#include "Ability/CAttributeSet.h"

UCAbilitySystemComponent::UCAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetHealthAttribute()).AddUObject(this, &UCAbilitySystemComponent::HealthUpdated);
}

void UCAbilitySystemComponent::ApplyInitialEffects()
{
	if (!GetOwner()->HasAuthority())
		return;

	for (const TSubclassOf<UGameplayEffect>& InitialEffect : InitialEffects) 
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(InitialEffect, 1, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);
	}
}

void UCAbilitySystemComponent::GiveInitialAbilities()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 0, (uint32)AbilityPair.Key));
	}

	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : BasicAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 0, (uint32)AbilityPair.Key));
	}
}

void UCAbilitySystemComponent::AuthApplyGameplayEffect(const TSubclassOf<UGameplayEffect>& EffectToApply) 
{
	if (GetOwner() && GetOwner()->HasAuthority() && EffectToApply)
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(EffectToApply, 1, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);
	}
}

void UCAbilitySystemComponent::ApplyFullStatEffect()
{
	AuthApplyGameplayEffect(FullStatEffect);
}

void UCAbilitySystemComponent::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (ChangeData.NewValue == 0)
	{
		AuthApplyGameplayEffect(DeathEffect);
	}
}
