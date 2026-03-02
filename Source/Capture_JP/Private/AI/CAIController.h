// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "CAIController.generated.h"

/**
 * 
 */
UCLASS()
class ACAIController : public AAIController
{
	GENERATED_BODY()
public:
	ACAIController();
	
	virtual void OnPossess(APawn* NewPawn) override;
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Perception")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category = "Perception")
	FName TargetBlackboardKeyName = "Target";

	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	class UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	class UAISenseConfig_Sight* SightConfig;

	UFUNCTION()
	void TargetPerceptionUpdated(AActor* Target, FAIStimulus Stimulus);

	void SetCurrentTarget(AActor* NewTarget);

	UObject* GetCurrentTarget() const;

	UFUNCTION()
	void TargetForgotten(AActor* ForgottenTarget);

	AActor* GetNextTarget() const;

	void ForgetActorImmediatleyIfInvisible(const AActor* TargetActor);
};
