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
	}

	LoginButton->OnClicked.AddDynamic(this, &UMainMenuWidget::LoginButtonClicked);
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
