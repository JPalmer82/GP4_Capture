// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/CGameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GA_GroundBlast.generated.h"

/**
 * 
 */
UCLASS()
class UGA_GroundBlast : public UCGameplayAbility
{
	GENERATED_BODY()
	
public:

	UGA_GroundBlast();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetMaxDistance = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetingAreaRadius = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* TargetingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class ATA_GroundPick> GroundPickTargetActorClass;

	UFUNCTION()
	void TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION()
	void TargetCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
};
