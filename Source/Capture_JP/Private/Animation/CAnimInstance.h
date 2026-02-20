// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UCAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	// the below functions are the native overrides for each phase
	// Native initialization override point
	virtual void NativeInitializeAnimation() override;

	// Native update override point. It is usually a good idea to simply gather data in this step and 
	// for the bulk of the work to be done in NativeThreadSafeUpdateAnimation.
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsMoving() const { return Speed != 0.0f; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsNotMoving() const { return Speed == 0.0f; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsOnGround() const { return !bIsFalling; }

private:

	UPROPERTY()
	class ACharacter* OwnerCharacter;

	UPROPERTY()
	class UCharacterMovementComponent* OwnerCharacterMovementComponent;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	float Speed;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	float YawSpeed;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	float SmoothedYawSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float SmoothYawSpeedLerpRate = 4.0f;

	FRotator PrevBodyRotation;
};
