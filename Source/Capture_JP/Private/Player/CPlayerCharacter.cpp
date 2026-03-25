// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "GameFramework/SpringArmComponent.h"

#include "GameFramework/PlayerController.h"

#include "Capture_JP/Capture_JP.h"

ACPlayerCharacter::ACPlayerCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("Camera Boom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->ProbeChannel = ECC_SpringArm;
	CameraBoom->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>("View Camera");
	ViewCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
}

void ACPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	APlayerController* PlayerController = GetController<APlayerController>();
	if (IsValid(PlayerController)) 
	{
		EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (EnhancedInputSubsystem)
		{
			EnhancedInputSubsystem->ClearAllMappings();
			EnhancedInputSubsystem->AddMappingContext(GameplayInputMappingContext, 0);
			EnhancedInputSubsystem->AddMappingContext(StaticInputMappingContext, 1);
		}
	}
}

void ACPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent) 
	{
		EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::HandleMoveInput);
		EnhancedInputComponent->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::HandleLookInput);

		for (const TPair<ECAbilityInputID, UInputAction*>& AbilityInputPair : AbilityInputMap)
		{
			EnhancedInputComponent->BindAction(
				AbilityInputPair.Value, 
				ETriggerEvent::Triggered, 
				this, 
				&ACPlayerCharacter::HandleAbilityInput,
				AbilityInputPair.Key
			);
		}
	}
}

void ACPlayerCharacter::OnStunStatChanged(bool bNewStunStatChanged)
{
	if (!IsCharacterDead())
	{
		SetGameplayInputEnabled(!bNewStunStatChanged);
	}
}

void ACPlayerCharacter::SetGameplayInputEnabled(bool bGameplayInputEnabled)
{
	if (EnhancedInputSubsystem)
	{
		EnhancedInputSubsystem->RemoveMappingContext(GameplayInputMappingContext);
		if (bGameplayInputEnabled)
		{
			EnhancedInputSubsystem->AddMappingContext(GameplayInputMappingContext, 0);
		}
	}
}

void ACPlayerCharacter::HandleMoveInput(const FInputActionValue& InputActionValue)
{
	FVector2D InputVal = InputActionValue.Get<FVector2D>();
	//UE_LOG(LogTemp, Warning, TEXT("Move Input is: %s"), *(InputVal.ToString()))

	FVector RightDir = ViewCamera->GetRightVector();
	FVector FwdDir = FVector::CrossProduct(RightDir, FVector::UpVector);

	AddMovementInput(RightDir * InputVal.X + FwdDir * InputVal.Y);
}

void ACPlayerCharacter::HandleLookInput(const FInputActionValue& InputActionValue)
{
	FVector2D InputVal = InputActionValue.Get<FVector2D>();

	AddControllerYawInput(InputVal.X);
	AddControllerPitchInput(InputVal.Y);
}

void ACPlayerCharacter::HandleAbilityInput(const FInputActionValue& InputActionValue, ECAbilityInputID AbilityInputID)
{
	if (IsCharacterDead())
	{
		return;
	}

	bool bPressed = InputActionValue.Get<bool>();
	if (bPressed)
	{
		GetAbilitySystemComponent()->AbilityLocalInputPressed((int32)AbilityInputID);
	}
	else
	{
		GetAbilitySystemComponent()->AbilityLocalInputReleased((int32)AbilityInputID);
	}
}

void ACPlayerCharacter::OnDead()
{
	//GetAbilitySystemComponent()->CancelAllAbilities();
}

void ACPlayerCharacter::OnRespawn()
{
	SetGameplayInputEnabled(true);
}

void ACPlayerCharacter::AimStateChanged(bool bNewIsAiming)
{
	GetCharacterMovement()->bOrientRotationToMovement = !bNewIsAiming;
	bUseControllerRotationYaw = bNewIsAiming;

	if (GetController() && GetController()->IsLocalPlayerController())
	{
		FVector CameraOffsetGoal = bNewIsAiming ? CameraAimLocalOffset : FVector::ZeroVector;
		GetWorldTimerManager().ClearTimer(CameraLerpTimerHandle);
		CameraLerpTimerHandle = GetWorldTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateUObject(this, &ACPlayerCharacter::UpdateCameraLocalOffset, CameraOffsetGoal)
		);
	}
}

void ACPlayerCharacter::UpdateCameraLocalOffset(FVector Goal)
{
	if (FVector::Dist(ViewCamera->GetRelativeLocation(), Goal) < 0.1f)
	{
		ViewCamera->SetRelativeLocation(Goal);
		return;
	}

	float LerpAlpha = CameraAimLerpingSpeed * GetWorld()->GetDeltaSeconds();

	FVector NewRelativeLocation = FMath::Lerp(ViewCamera->GetRelativeLocation(), Goal, LerpAlpha);

	ViewCamera->SetRelativeLocation(NewRelativeLocation);
	CameraLerpTimerHandle = GetWorldTimerManager().SetTimerForNextTick(
		FTimerDelegate::CreateUObject(this, &ACPlayerCharacter::UpdateCameraLocalOffset, Goal)
	);
}
