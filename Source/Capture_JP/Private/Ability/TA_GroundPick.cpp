// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/TA_GroundPick.h"
#include "Engine/OverlapResult.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"

ATA_GroundPick::ATA_GroundPick()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATA_GroundPick::ConfirmTargetingAndContinue()
{
	TArray<FOverlapResult> OverlapResults;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	GetWorld()->OverlapMultiByObjectType(
								OverlapResults, 
								GetActorLocation(), 
								FQuat::Identity, 
								ObjectQueryParams, 
								FCollisionShape::MakeSphere(TargetingAreaRadius)
		);

	TSet<AActor*> Targets;
	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		if (!IsTargetValidForAttitudes(*OverlapResult.GetActor()))
		{
			continue;
		}

		Targets.Add(OverlapResult.GetActor());
	}

	TargetDataReadyDelegate.Broadcast(UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(Targets.Array(), false));
}

bool ATA_GroundPick::IsTargetValidForAttitudes(const AActor& TargetCandidate)
{
	IGenericTeamAgentInterface* OwnerTeamInterface = nullptr;
	if (!OwningAbility)
	{
		return true;
	}

	OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(OwningAbility->GetAvatarActorFromActorInfo());
	if (!OwnerTeamInterface)
	{
		return true;
	}

	return TargetingWithAttitudes.Contains(OwnerTeamInterface->GetTeamAttitudeTowards(TargetCandidate));
}

void ATA_GroundPick::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TraceTargetingLocation();
	SetActorLocation(TargetingLocation);
}

void ATA_GroundPick::TraceTargetingLocation()
{
	TargetingLocation = GetActorLocation();

	if (!PrimaryPC)
	{
		return;
	}

	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;

	PrimaryPC->GetPlayerViewPoint(PlayerViewLocation, PlayerViewRotation);

	FVector TargetEnd = PlayerViewLocation + PlayerViewRotation.Vector() * TargetMaxDistance;

	FHitResult TraceResult;

	GetWorld()->LineTraceSingleByChannel(TraceResult, PlayerViewLocation, TargetEnd, ECC_Visibility);

	if (!TraceResult.bBlockingHit)
	{
		GetWorld()->LineTraceSingleByChannel(
			TraceResult, 
			TargetEnd, 
			TargetEnd + FVector::DownVector * TNumericLimits<float>::Max(), 
			ECC_Visibility);
	}

	if (!TraceResult.bBlockingHit)
	{
		return;
	}

	TargetingLocation = TraceResult.ImpactPoint;
}
