// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "WaitingWidget.generated.h"

/**
 * 
 */
UCLASS()
class UWaitingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void ConfigureWaiting(const FString& WaitingTextString, bool bAllowCancel);
	FOnButtonClickedEvent& ClearAndGetButtonClickedDelegate();

private:
	UPROPERTY(meta=(BindWidget))
	class UButton* CancelButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WaitingInfoText;
	
};
