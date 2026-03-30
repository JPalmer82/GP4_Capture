// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ListView.h"
#include "Ability/CAbilityInputID.h"
#include "AbilityListView.generated.h"

/**
 * 
 */
UCLASS()
class UAbilityListView : public UListView
{
	GENERATED_BODY()
	
public:
	void ConfigureWithAbilities(const TMap<ECAbilityInputID, TSubclassOf<class UGameplayAbility>>& AbilitiesMap);

private:
	UPROPERTY(EditAnywhere, Category = "Ability")
	UDataTable* AbilityInfoDataTable;

	const struct FAbilityWidgetInfo* GetAbilityWidgetInfoForAbility(const class UGameplayAbility* Ability);

	void AbilityWidgetGenerated(UUserWidget& WidgetGenerated);
};
