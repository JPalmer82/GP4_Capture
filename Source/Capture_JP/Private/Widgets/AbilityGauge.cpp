// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilityGauge.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Ability/CAbilitySystemStatics.h"

void UAbilityGauge::NativeConstruct()
{
	Super::NativeConstruct();
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerASC)
	{
		OwnerASC->AbilityCommittedCallbacks.AddUObject(this, &UAbilityGauge::AbilityCommitted);
	}
}

void UAbilityGauge::AbilityCommitted(UGameplayAbility* Ability)
{
	if (Ability->GetClass()->GetDefaultObject() != AbilityCDO)
	{
		return;
	}

	float CooldownDuration, CooldownRemaining;
	Ability->GetCooldownTimeRemainingAndDuration(
		Ability->GetCurrentAbilitySpecHandle(), Ability->GetCurrentActorInfo(), CooldownRemaining, CooldownDuration);

	CachedCooldownDuration = CooldownDuration;
	CachedCooldownTimeRemaining = CooldownRemaining;

	CooldownCounterText->SetVisibility(ESlateVisibility::Visible);

	GetWorld()->GetTimerManager().SetTimer(CooldownDurationTimerHandle, this, &UAbilityGauge::CooldownFinished, CooldownDuration);
	GetWorld()->GetTimerManager().SetTimer(CooldownUpdateTimerHandle, this, &UAbilityGauge::UpdateCooldown, CooldownUpdateInterval, true, 0);
}

void UAbilityGauge::CooldownFinished()
{
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
	GetWorld()->GetTimerManager().ClearTimer(CooldownUpdateTimerHandle);

	CachedCooldownDuration = CachedCooldownTimeRemaining = 0.0f;
	Icon->GetDynamicMaterial()->SetScalarParameterValue("Percent", 1);
}

void UAbilityGauge::UpdateCooldown()
{
	static FNumberFormattingOptions NumberFormattingOptions;
	NumberFormattingOptions.MaximumFractionalDigits = CachedCooldownTimeRemaining >= 1 ? 0 : 1;

	CooldownCounterText->SetText(FText::AsNumber(CachedCooldownTimeRemaining, &NumberFormattingOptions));
	CachedCooldownTimeRemaining -= CooldownUpdateInterval;
	Icon->GetDynamicMaterial()->SetScalarParameterValue("Percent", 1 - (CachedCooldownTimeRemaining / CachedCooldownDuration));
}

void UAbilityGauge::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	AbilityCDO = Cast<UGameplayAbility>(ListItemObject);

	float CooldownDuration = UCAbilitySystemStatics::GetStaticCooldownDuration(AbilityCDO);
	float Cost = UCAbilitySystemStatics::GetStaticCost(AbilityCDO);

	CooldownDurationText->SetText(FText::AsNumber(CooldownDuration));
	CostText->SetText(FText::AsNumber(Cost));
}

void UAbilityGauge::ConfigureWithWidgetInfo(const FAbilityWidgetInfo* WidgetInfo)
{
	if (WidgetInfo)
	{
		Icon->GetDynamicMaterial()->SetTextureParameterValue("Icon", WidgetInfo->Icon.LoadSynchronous());
	}
}
