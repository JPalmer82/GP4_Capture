// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GenericTeamAgentInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Ability/CGameplayTypes.h"

ACAIController::ACAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("Perception Component");
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight Config");

	SightConfig->SightRadius = 1000.0f;
	SightConfig->LoseSightRadius = 1200.0f;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;

	SightConfig->SetMaxAge(5.0f);

	SightConfig->PeripheralVisionAngleDegrees = 60.0f;

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ACAIController::TargetPerceptionUpdated);
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ACAIController::TargetForgotten);
}

void ACAIController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	IGenericTeamAgentInterface* PawnTeamInterface = Cast<IGenericTeamAgentInterface>(NewPawn);
	SetGenericTeamId(FGenericTeamId(0));
	if (PawnTeamInterface)
	{
		PawnTeamInterface->SetGenericTeamId(GetGenericTeamId());
	}
}

void ACAIController::BeginPlay()
{
	Super::BeginPlay();
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
}

void ACAIController::TargetPerceptionUpdated(AActor* Target, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if (!GetCurrentTarget())
		{
			SetCurrentTarget(Target);
		}
	}
	else
	{
		ForgetActorImmediatleyIfInvisible(Target);
	}
}

void ACAIController::SetCurrentTarget(AActor* NewTarget)
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		if (NewTarget)
		{
			BlackboardComponent->SetValueAsObject(TargetBlackboardKeyName, NewTarget);
		}
		else
		{
			BlackboardComponent->ClearValue(TargetBlackboardKeyName);
		}
	}
}

UObject* ACAIController::GetCurrentTarget() const
{
	if (const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		return BlackboardComponent->GetValueAsObject(TargetBlackboardKeyName);
	}

	return nullptr;
}

void ACAIController::TargetForgotten(AActor* ForgottenTarget)
{
	if (GetCurrentTarget() == ForgottenTarget)
	{
		SetCurrentTarget(GetNextTarget());
	}
}

AActor* ACAIController::GetNextTarget() const
{
	TArray<AActor*> HostileActors;
	AIPerceptionComponent->GetHostileActors(HostileActors);

	float CurrentMinDistance = TNumericLimits<float>::Max();
	AActor* ClosestTarget = nullptr;

	for (AActor* HostileActor : HostileActors)
	{
		float Distance = FVector::Dist(GetPawn()->GetActorLocation(), HostileActor->GetActorLocation());
		if (Distance < CurrentMinDistance)
		{
			CurrentMinDistance = Distance;
			ClosestTarget = HostileActor;
		}
	}

	return ClosestTarget;
}

void ACAIController::ForgetActorImmediatleyIfInvisible(const AActor* TargetActor)
{
	const IAbilitySystemInterface* TargetAbilitySystemInterface = Cast<IAbilitySystemInterface>(TargetActor);
	if (!TargetAbilitySystemInterface)
	{
		return;
	}

	const UAbilitySystemComponent* TargetASC = TargetAbilitySystemInterface->GetAbilitySystemComponent();
	if (!TargetASC)
	{
		return;
	}

	if (!TargetASC->HasMatchingGameplayTag(TAG_STAT_Invisible))
	{
		return;
	}

	for (auto TargetPerceptualData = AIPerceptionComponent->GetPerceptualDataIterator(); 
		TargetPerceptualData; 
		++TargetPerceptualData)
	{
		if (TargetPerceptualData->Key != TargetActor)
		{
			continue;
		}
		for (FAIStimulus& Stimuli : TargetPerceptualData->Value.LastSensedStimuli)
		{
			Stimuli.SetStimulusAge(TNumericLimits<float>::Max());
		}
	}
}
