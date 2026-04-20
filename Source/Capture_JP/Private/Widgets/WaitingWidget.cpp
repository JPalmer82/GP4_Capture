// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/WaitingWidget.h"
#include "Components/TextBlock.h"

void UWaitingWidget::ConfigureWaiting(const FString& WaitingTextString, bool bAllowCancel)
{
	WaitingInfoText->SetText(FText::FromString(WaitingTextString));
	CancelButton->SetVisibility(bAllowCancel ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	//CancelButton->SetIsEnabled(bAllowCancel);
}

FOnButtonClickedEvent& UWaitingWidget::ClearAndGetButtonClickedDelegate()
{
	CancelButton->OnClicked.Clear();
	return CancelButton->OnClicked;
}
