// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PatrollingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UPatrollingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AActor* GetNextPatrolPoint();

private:
	UPROPERTY(EditAnywhere, Category = "Patrolling")
	TArray<class ATargetPoint*> PatrolPoints;

	int NextPatrolPointIndex = 0;
};
