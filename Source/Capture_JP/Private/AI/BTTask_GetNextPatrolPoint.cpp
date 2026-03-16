// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GetNextPatrolPoint.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AiController.h"
#include "AI/PatrollingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GetNextPatrolPoint::UBTTask_GetNextPatrolPoint()
{
	NodeName = "GetNextPatrolPoint";
}

EBTNodeResult::Type UBTTask_GetNextPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* OwnerAIC = OwnerComp.GetAIOwner();
	if (!OwnerAIC)
	{
		return EBTNodeResult::Failed;
	}

	UPatrollingComponent* PatrollingComponent = OwnerAIC->GetPawn()->GetComponentByClass<UPatrollingComponent>();
	if (!PatrollingComponent)
	{
		return EBTNodeResult::Failed;
	}

	AActor* NextPatrolPoint = PatrollingComponent->GetNextPatrolPoint();
	UBlackboardComponent* OwnerBlackboardComponent = OwnerComp.GetBlackboardComponent();
	OwnerBlackboardComponent->SetValueAsObject(GetSelectedBlackboardKey(), NextPatrolPoint);
	
	return EBTNodeResult::Succeeded;
}
