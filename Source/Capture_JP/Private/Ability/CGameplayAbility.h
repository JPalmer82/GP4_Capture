// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class UCGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
protected:
	class UAnimInstance* GetOwnerAnimInst() const;

	TArray<FHitResult> GetHitResultFromTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, 
		float DetectionRadius, bool bIgnoreSelf = true, bool bDrawDebug = true) const;

	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool bShouldDrawDebug = false;
};
