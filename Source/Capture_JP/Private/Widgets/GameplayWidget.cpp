// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameplayWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Ability/CAttributeSet.h"

#include "Widgets/AbilityListView.h"

#include "Widgets/ValueGauge.h"

void UGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();
	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	HealthBar->BindToGameplayAttribute(OwnerAbilitySystemComponent, UCAttributeSet::GetHealthAttribute(), UCAttributeSet::GetMaxHealthAttribute());
	ManaBar->BindToGameplayAttribute(OwnerAbilitySystemComponent, UCAttributeSet::GetManaAttribute(), UCAttributeSet::GetMaxManaAttribute());

	/*UCAbilitySystemComponent* CAbilitySystemComponent = Cast<UCAbilitySystemComponent>(OwnerAbilitySystemComponent);
	if (CAbilitySystemComponent)
	{
		AbilityListView->ConfigureWithAbilities(CAbilitySystemComponent->GetAbilities());
	}*/
}

void UGameplayWidget::ConfigureWithAbilities(const TMap<ECAbilityInputID, TSubclassOf<class UGameplayAbility>>& AbilitiesMap)
{
	AbilityListView->ConfigureWithAbilities(AbilitiesMap);
}
