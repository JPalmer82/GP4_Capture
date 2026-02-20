// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ValueGauge.h"

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UValueGauge::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (ProgressBar)
	{
		ProgressBar->SetFillColorAndOpacity(BarColor);
	}
}

void UValueGauge::SetValue(float Value, float MaxValue)
{
	CachedValue = Value;
	CachedMaxValue = MaxValue;

	if (MaxValue == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Max Value is: 0, can't calculate value for value gauge"), *(GetName()));
		return;
	}

	ProgressBar->SetPercent(Value / MaxValue);

	static FNumberFormattingOptions FormattingOptions = FNumberFormattingOptions().SetMaximumFractionalDigits(0);

	ValueText->SetText(
		FText::Format(
			FTextFormat::FromString("{0}/{1}"),
			FText::AsNumber(Value, &FormattingOptions),
			FText::AsNumber(MaxValue, &FormattingOptions))
	);
}

void UValueGauge::ValueUpdated(const FOnAttributeChangeData& NewValue)
{
	SetValue(NewValue.NewValue, CachedMaxValue);
}

void UValueGauge::MaxValueUpdated(const FOnAttributeChangeData& NewMaxValue)
{
	SetValue(CachedValue, NewMaxValue.NewValue);
}

void UValueGauge::BindToGameplayAttribute(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute)
{
	bool bFound = false;
	float Value = AbilitySystemComponent->GetGameplayAttributeValue(Attribute, bFound);
	float MaxValue = AbilitySystemComponent->GetGameplayAttributeValue(MaxAttribute, bFound);

	if (bFound) 
	{
		SetValue(Value, MaxValue);
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UValueGauge::ValueUpdated);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(this, &UValueGauge::MaxValueUpdated);
}
