// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/CGameplayAbility.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UAnimInstance* UCGameplayAbility::GetOwnerAnimInst() const
{
	USkeletalMeshComponent* OwnerSkeletalMesh = GetOwningComponentFromActorInfo();

	if (!OwnerSkeletalMesh)
	{
		return nullptr;
	}

	return OwnerSkeletalMesh->GetAnimInstance();
}

TArray<FHitResult> UCGameplayAbility::GetHitResultFromTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float DetectionRadius, bool bIgnoreSelf, ETeamAttitude::Type TargetTeamAttitude, bool bDrawDebug) const
{
	TArray<FHitResult> OutResult;
	TSet<AActor*> AlreadyDetectedActors;

	TArray<TEnumAsByte<EObjectTypeQuery>> QueryTypes;
	QueryTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	if (bIgnoreSelf)
	{
		ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	}

	EDrawDebugTrace::Type DebugTrackType = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	IGenericTeamAgentInterface* AvatarTeamInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());

	for (const TSharedPtr<FGameplayAbilityTargetData>& TargetData : TargetDataHandle.Data)
	{
		FVector StartLoc = TargetData->GetOrigin().GetLocation();
		FVector EndLoc = TargetData->GetEndPoint();

		TArray<FHitResult> HitResults;
		UKismetSystemLibrary::SphereTraceMultiForObjects(this, StartLoc, EndLoc, 
			DetectionRadius, QueryTypes, false, ActorsToIgnore, DebugTrackType, HitResults, false);

		for (const FHitResult& HitResult : HitResults)
		{
			if (AlreadyDetectedActors.Contains(HitResult.GetActor()))
			{
				continue;
			}

			if (AvatarTeamInterface && AvatarTeamInterface->GetTeamAttitudeTowards(*HitResult.GetActor()) == TargetTeamAttitude)
			{
				OutResult.Add(HitResult);
			}

			AlreadyDetectedActors.Add(HitResult.GetActor());
		}
	}

	return OutResult;
}
