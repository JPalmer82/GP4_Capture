// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilityGauge.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Abilities/GameplayAbility.h"

void UAbilityGauge::NativeConstruct()
{
	Super::NativeConstruct();
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
}

void UAbilityGauge::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	AbilityCDO = Cast<UGameplayAbility>(ListItemObject);
}

void UAbilityGauge::ConfigureWithWidgetInfo(const FAbilityWidgetInfo* WidgetInfo)
{
	if (WidgetInfo)
	{
		Icon->GetDynamicMaterial()->SetTextureParameterValue("Icon", WidgetInfo->Icon.LoadSynchronous());
	}
}
