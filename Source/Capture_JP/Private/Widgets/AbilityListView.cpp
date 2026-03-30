// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilityListView.h"
#include "Abilities/GameplayAbility.h"
#include "Widgets/AbilityGauge.h"

void UAbilityListView::ConfigureWithAbilities(const TMap<ECAbilityInputID, TSubclassOf<class UGameplayAbility>>& AbilitiesMap)
{
	OnEntryWidgetGenerated().AddUObject(this, &UAbilityListView::AbilityWidgetGenerated);

	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : AbilitiesMap)
	{
		AddItem(AbilityPair.Value->GetDefaultObject());
	}
}

const FAbilityWidgetInfo* UAbilityListView::GetAbilityWidgetInfoForAbility(const UGameplayAbility* Ability)
{
	if (AbilityInfoDataTable)
	{
		TArray<FAbilityWidgetInfo*> AbilityWidgetInfos;
		AbilityInfoDataTable->GetAllRows("Finding Ability Widget Info Rows", AbilityWidgetInfos);

		for (const FAbilityWidgetInfo* WidgetInfo : AbilityWidgetInfos)
		{
			if (WidgetInfo->AbilityClass->GetDefaultObject() == Ability)
			{
				return WidgetInfo;
			}
		}
	}

	return nullptr;
}

void UAbilityListView::AbilityWidgetGenerated(UUserWidget& WidgetGenerated)
{
	UAbilityGauge* GeneratedAbilityGauge = Cast<UAbilityGauge>(&WidgetGenerated);
	if (GeneratedAbilityGauge)
	{
		GeneratedAbilityGauge->ConfigureWithWidgetInfo(GetAbilityWidgetInfoForAbility(GeneratedAbilityGauge->GetListItem<UGameplayAbility>()));
	}
}
