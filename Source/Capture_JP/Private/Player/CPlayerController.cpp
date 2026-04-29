// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerController.h"
#include "Player/CPlayerCharacter.h"

#include "Widgets/GameplayWidget.h"
#include "Net/UnrealNetwork.h"

void ACPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	CPlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
	if (CPlayerCharacter)
	{
		CPlayerCharacter->ServerSideInit();
		CPlayerCharacter->SetGenericTeamId(TeamId);
	}
}

void ACPlayerController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);

	SetInputMode(FInputModeGameAndUI());
	CPlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
	if (CPlayerCharacter)
	{
		CPlayerCharacter->ClientSideInit();
	}

	SpawnGameplayWidget();
}

void ACPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACPlayerController, TeamId);
}

void ACPlayerController::SpawnGameplayWidget()
{
	if (GameplayWidgetClass)
	{
		GameplayWidget = CreateWidget<UGameplayWidget>(this, GameplayWidgetClass);
		GameplayWidget->AddToViewport();
		GameplayWidget->ConfigureWithAbilities(CPlayerCharacter->GetAbilities());
	}
}
