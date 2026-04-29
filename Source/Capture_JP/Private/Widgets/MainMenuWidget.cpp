// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MainMenuWidget.h"
#include "Components/Button.h"
#include "Framework/CGameInstance.h"
#include "Components/WidgetSwitcher.h"
#include "Widgets/WaitingWidget.h"
#include "Widgets/MatchMakingWidget.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CGameInstance = GetGameInstance<UCGameInstance>();
	if (CGameInstance)
	{
		//...
		CGameInstance->OnLoginCompleted.AddUObject(this, &UMainMenuWidget::LoginCompleted);
		if (CGameInstance->IsClientLoggedIn())
		{
			LoginCompleted(true, CGameInstance->GetPlayerNickName());
		}
	}

	LoginButton->OnClicked.AddDynamic(this, &UMainMenuWidget::LoginButtonClicked);

	MatchMakingWidget->GetNewSessionButtonClickedEvent().AddDynamic(this, &UMainMenuWidget::WaitForNewSessionCreate);
	MatchMakingWidget->GetFindAndJoinSessionButtonClickedEvent().AddDynamic(this, &UMainMenuWidget::WaitForFindAndJoinSession);
}

FReply UMainMenuWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("Focus added to path"))
	UWidget* ActiveWidget = MainSwitcher->GetActiveWidget();
	if (ActiveWidget)
	{
		ActiveWidget->SetKeyboardFocus();
		return FReply::Handled();
	}

	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UMainMenuWidget::LoginButtonClicked()
{
	if (CGameInstance)
	{
		CGameInstance->ClientAuthPortalLogin();
		MainSwitcher->SetActiveWidget(WaitingWidget);
		WaitingWidget->ConfigureWaiting("Waiting", false);
	}
}

void UMainMenuWidget::LoginCompleted(bool bWasSuccessful, const FString& PlayerNickname)
{
	if (bWasSuccessful)
	{
		MainSwitcher->SetActiveWidget(MatchMakingWidget);
		MatchMakingWidget->SetKeyboardFocus();
	}
}

void UMainMenuWidget::WaitForNewSessionCreate()
{
	MainSwitcher->SetActiveWidget(WaitingWidget);
	WaitingWidget->ConfigureWaiting("Creating Session", true);
	WaitingWidget->ClearAndGetButtonClickedDelegate().AddDynamic(this, &UMainMenuWidget::BackToMatchMaking);
	WaitingWidget->SetKeyboardFocus();
}

void UMainMenuWidget::WaitForFindAndJoinSession()
{
	MainSwitcher->SetActiveWidget(WaitingWidget);
	WaitingWidget->ConfigureWaiting("Finding Sessions", true);
	WaitingWidget->ClearAndGetButtonClickedDelegate().AddDynamic(this, &UMainMenuWidget::BackToMatchMaking);
	WaitingWidget->SetKeyboardFocus();
}

void UMainMenuWidget::BackToMatchMaking()
{
	MainSwitcher->SetActiveWidget(MatchMakingWidget);
	MatchMakingWidget->SetKeyboardFocus();
}
