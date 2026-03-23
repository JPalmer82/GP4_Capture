// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GenericTeamAgentInterface.h"
#include "TA_GroundPick.generated.h"

/**
 * 
 */
UCLASS()
class ATA_GroundPick : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
public:
	ATA_GroundPick();
	virtual void ConfirmTargetingAndContinue() override;
	FORCEINLINE void AddTargetingAttitude(ETeamAttitude::Type NewAttitude) { TargetingWithAttitudes.AddUnique(NewAttitude); }
	FORCEINLINE void SetTargetMaxDistance(float NewTargetMaxDistance) { TargetMaxDistance = NewTargetMaxDistance;  }
	FORCEINLINE void SetTargetAreaRadius(float NewTargetingAreaRadius) { TargetingAreaRadius = NewTargetingAreaRadius; }

	bool IsTargetValidForAttitudes(const AActor& TargetCandidate);

protected:
	virtual void Tick(float DeltaSeconds) override;

private:
	FVector TargetingLocation;
	void TraceTargetingLocation();

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetMaxDistance = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetingAreaRadius = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TArray<TEnumAsByte<ETeamAttitude::Type>> TargetingWithAttitudes;
};
