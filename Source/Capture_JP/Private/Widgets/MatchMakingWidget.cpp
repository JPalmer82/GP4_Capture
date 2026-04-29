// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MatchMakingWidget.h"
#include "Framework/CGameInstance.h"
#include "Components/Button.h"


void UMatchMakingWidget::NativeConstruct()
{
	CGameInstance = GetGameInstance<UCGameInstance>();
	if (CGameInstance)
	{
		//...
	}

	FindAndJoinSessionButton->OnClicked.AddDynamic(this, &UMatchMakingWidget::FindAndJoinSessionButtonClicked);
	CreateNewSessionButton->OnClicked.AddDynamic(this, &UMatchMakingWidget::CreateNewSessionButtonClicked);
}

FOnButtonClickedEvent& UMatchMakingWidget::GetNewSessionButtonClickedEvent()
{
	return CreateNewSessionButton->OnClicked;
}

FOnButtonClickedEvent& UMatchMakingWidget::GetFindAndJoinSessionButtonClickedEvent()
{
	return FindAndJoinSessionButton->OnClicked;
}

void UMatchMakingWidget::CreateNewSessionButtonClicked()
{
	if (CGameInstance)
	{
		CGameInstance->RequestCreateNewSession();
	}
}

void UMatchMakingWidget::FindAndJoinSessionButtonClicked()
{
	if (CGameInstance)
	{
		CGameInstance->TryFindAndJoinSession();
	}
}
