// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_PressAbilityInput.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

EBTNodeResult::Type UBTTask_PressAbilityInput::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Warning, TEXT("Trying to press ability input: %d"), (int32)InputId)
	const AAIController* OwnerAIController =	OwnerComp.GetAIOwner();
	if (!OwnerAIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* OwnerPawn = OwnerAIController->GetPawn();
	if (!OwnerPawn)
	{
		return EBTNodeResult::Failed;
	}

	UAbilitySystemComponent* OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerPawn);
	if (!OwnerAbilitySystemComponent)
	{
		return EBTNodeResult::Failed;
	}

	OwnerAbilitySystemComponent->AbilityLocalInputPressed((int32)InputId);
	return EBTNodeResult::Succeeded;
}
