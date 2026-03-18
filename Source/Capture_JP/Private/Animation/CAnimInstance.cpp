// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Ability/CGameplayTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UCAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		OwnerCharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
	}

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TryGetPawnOwner());
	if (OwnerASC)
	{
		OwnerASC->RegisterGameplayTagEvent(TAG_STAT_Aiming).AddUObject(this, &UCAnimInstance::OnAimTagChanged);
	}
}

void UCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (OwnerCharacter)
	{
		FVector Velocity = OwnerCharacter->GetVelocity();
		Speed = Velocity.Length();

		FRotator BodyRotation = OwnerCharacter->GetActorRotation();

		//this gives us the differences of the 2 rotations
		FRotator RotationDelta = UKismetMathLibrary::NormalizedDeltaRotator(BodyRotation, PrevBodyRotation);
		PrevBodyRotation = BodyRotation;

		YawSpeed = RotationDelta.Yaw / DeltaSeconds;
		SmoothedYawSpeed = UKismetMathLibrary::FInterpTo(SmoothedYawSpeed, YawSpeed, DeltaSeconds, SmoothYawSpeedLerpRate);

		FRotator BaseAimRotation = OwnerCharacter->GetBaseAimRotation();
		FVector AimRightDir = BaseAimRotation.Quaternion().GetRightVector();
		FVector AimForwardDir = AimRightDir.Cross(FVector::UpVector);

		ForwardSpeed = Velocity.Dot(AimForwardDir);
		RightSpeed = Velocity.Dot(AimRightDir);
	}

	if (OwnerCharacterMovementComponent)
	{
		bIsFalling = OwnerCharacterMovementComponent->IsFalling();
	}
}

bool UCAnimInstance::ShouldDoFullBody() const
{
	return IsNotMoving() && !bIsAiming;
}

void UCAnimInstance::OnAimTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bIsAiming = NewCount > 0;
}
