// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ValueGauge.generated.h"

class UAbilitySystemComponent;
struct FGameplayAttribute;
/**
 * 
 */
UCLASS()
class UValueGauge : public UUserWidget
{
	GENERATED_BODY()

public:
	// the same as the construction script in blueprint
	virtual void NativePreConstruct() override;
	void BindToGameplayAttribute(UAbilitySystemComponent* AbilitySystemComponent, 
		const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute);

	void SetValue(float Value, float MaxValue);
	
private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ValueText;

	UPROPERTY(EditAnywhere, Category = "Value Gauge")
	FLinearColor BarColor;

	void ValueUpdated(const struct FOnAttributeChangeData& NewValue);
	void MaxValueUpdated(const struct FOnAttributeChangeData& NewMaxValue);

	float CachedValue = 0.0f;
	float CachedMaxValue = 0.0f;
};
