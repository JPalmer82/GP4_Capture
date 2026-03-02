// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CCharacter.h"

#include "Ability/CAbilitySystemComponent.h"
#include "Ability/CAttributeSet.h"
#include "Ability/CGameplayTypes.h"

#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Widgets/OverheadStatusGauge.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACCharacter::ACCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 800.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 800.0f;

	CAbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>("CAbilitySystemComponent");
	CAttributeSet = CreateDefaultSubobject<UCAttributeSet>("CAttributeSet");

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	OverheadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("OverheadWidgetComponent");
	OverheadWidgetComponent->SetupAttachment(GetRootComponent());

	BindGameplayTagChangeEvents();

	PerceptionStimulusComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("PerceptionStimulusComponent");
}

void ACCharacter::ServerSideInit()
{
	CAbilitySystemComponent->InitAbilityActorInfo(this, this);
	CAbilitySystemComponent->ApplyInitialEffects();
	CAbilitySystemComponent->GiveInitialAbilities();
}

void ACCharacter::ClientSideInit()
{
	CAbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void ACCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACCharacter, TeamId);
}

// Called when the game starts or when spawned
void ACCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitializeOverheadWidget();

	if (GetController() && !GetController()->IsPlayerController())
	{
		ServerSideInit();
	}
	//gets the information of the character's mesh for later use in turning ragdoll off 
	MeshRelativeTransform = GetMesh()->GetRelativeTransform();

	PerceptionStimulusComponent->RegisterForSense(UAISense_Sight::StaticClass());
}

// Called every frame
void ACCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* ACCharacter::GetAbilitySystemComponent() const
{
	return CAbilitySystemComponent;
}

void ACCharacter::BindGameplayTagChangeEvents()
{
	GetAbilitySystemComponent()->RegisterGameplayTagEvent(TAG_STAT_Dead).AddUObject(this, &ACCharacter::DeathTagChanged);
}

void ACCharacter::DeathTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount != 0)
	{
		StartDeathSequence();
	}
	else
	{
		Respawn();
	}
}

bool ACCharacter::IsCharacterDead() const
{
	return GetAbilitySystemComponent()->HasAllMatchingGameplayTags(FGameplayTagContainer(TAG_STAT_Dead));
}

void ACCharacter::StartDeathSequence()
{
	UE_LOG(LogTemp, Warning, TEXT("Starting Death Sequence"))
	PlayDeathMontage();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	SetOverheadWidgetEnabled(false);
	GetAbilitySystemComponent()->CancelAllAbilities();
	SetPerceptionStimuliEnabled(false);
}

void ACCharacter::Respawn()
{
	UE_LOG(LogTemp, Warning, TEXT("Respawning"))
	OnRespawn();
	SetOverheadWidgetEnabled(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetMesh()->GetAnimInstance()->StopAllMontages(0.0f);
	SetEnableRagdoll(false);
	CAbilitySystemComponent->ApplyFullStatEffect();
	SetPerceptionStimuliEnabled(true);

	AController* OwningController = GetController();
	if (OwningController && OwningController->StartSpot.IsValid())
	{
		SetActorLocation(OwningController->StartSpot->GetActorLocation());
		SetActorRotation(OwningController->StartSpot->GetActorRotation());
	}
}

void ACCharacter::PlayDeathMontage()
{
	float MontageDuration = PlayAnimMontage(DeathMontage);
	GetWorldTimerManager().SetTimer(DeathAnimationFinishedTimerHandle, this, &ACCharacter::DeathAnimationFinished,
		MontageDuration + DeathAnimationDurationOffset
		);
}

void ACCharacter::SetEnableRagdoll(bool bEnableRagdoll)
{
	if (bEnableRagdoll)
	{
		GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);
	}
	else
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		GetMesh()->SetRelativeTransform(MeshRelativeTransform);
	}
}

void ACCharacter::DeathAnimationFinished()
{
	if (IsCharacterDead())
	{
		SetEnableRagdoll(true);
	}
}

void ACCharacter::InitializeOverheadWidget()
{
	if (GetController() && GetController()->IsPlayerController())
	{
		//We are the character played by the local player
		OverheadWidgetComponent->SetVisibility(false);
		return;
	}

	if (UOverheadStatusGauge* StatusGauge = Cast<UOverheadStatusGauge>(OverheadWidgetComponent->GetUserWidgetObject()))
	{
		StatusGauge->SetOwningAbilitySystemComponent(GetAbilitySystemComponent());
		OverheadWidgetComponent->SetVisibility(true);
	}
}

void ACCharacter::SetOverheadWidgetEnabled(bool bWidgetEnabled)
{
	OverheadWidgetComponent->SetHiddenInGame(!bWidgetEnabled);
}

void ACCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID;
	UE_LOG(LogTemp, Warning, TEXT("Team Id is set to: %d"), TeamId.GetId())
}

void ACCharacter::SetPerceptionStimuliEnabled(bool bStimuliEnabled)
{
	if (bStimuliEnabled)
	{
		PerceptionStimulusComponent->RegisterWithPerceptionSystem();
	}
	else
	{
		PerceptionStimulusComponent->UnregisterFromPerceptionSystem();
	}
}

