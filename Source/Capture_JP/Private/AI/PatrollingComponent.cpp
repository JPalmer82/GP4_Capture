// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/PatrollingComponent.h"
#include "Engine/TargetPoint.h"

AActor* UPatrollingComponent::GetNextPatrolPoint()
{
	if (NextPatrolPointIndex >= 0 && NextPatrolPointIndex < PatrolPoints.Num())
	{
		AActor* NextPatrolPoint = PatrolPoints[NextPatrolPointIndex];
		NextPatrolPointIndex = (NextPatrolPointIndex + 1) % PatrolPoints.Num();
		return NextPatrolPoint;
	}
	return nullptr;
}

