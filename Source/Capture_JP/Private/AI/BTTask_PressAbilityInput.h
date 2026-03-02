// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Ability/CAbilityInputID.h"
#include "BTTask_PressAbilityInput.generated.h"

/**
 * 
 */
UCLASS()
class UBTTask_PressAbilityInput : public UBTTaskNode
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	ECAbilityInputID InputId;
};
