// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ClearKey.h"
#include "BTTask_ClearKey.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_ClearKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
	return EBTNodeResult::Succeeded;
}
