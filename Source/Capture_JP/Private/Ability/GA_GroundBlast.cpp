// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_GroundBlast.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Ability/CGameplayTypes.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Ability/TA_GroundPick.h"
#include "AbilitySystemBlueprintLibrary.h"

UGA_GroundBlast::UGA_GroundBlast()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ActivationOwnedTags.AddTag(TAG_STAT_Aiming);
	BlockAbilitiesWithTag.AddTag(TAG_Combo);
}

void UGA_GroundBlast::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		K2_EndAbility();
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlayTargetingMontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargetingMontage);

	PlayTargetingMontageTask->OnBlendOut.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);
	PlayTargetingMontageTask->OnCompleted.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);
	PlayTargetingMontageTask->OnInterrupted.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);
	PlayTargetingMontageTask->OnCancelled.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);

	PlayTargetingMontageTask->ReadyForActivation();

	UAbilityTask_WaitTargetData* WaitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(
		this,
		NAME_None,
		EGameplayTargetingConfirmation::UserConfirmed,
		GroundPickTargetActorClass
	);

	WaitTargetDataTask->ValidData.AddDynamic(this, &UGA_GroundBlast::TargetReceived);
	WaitTargetDataTask->Cancelled.AddDynamic(this, &UGA_GroundBlast::TargetCancelled);
	WaitTargetDataTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor = nullptr;
	WaitTargetDataTask->BeginSpawningActor(this, GroundPickTargetActorClass, TargetActor);
	//Do Configuration Here
	ATA_GroundPick* GroundPickTargetActor = Cast<ATA_GroundPick>(TargetActor);
	if (GroundPickTargetActor)
	{
		GroundPickTargetActor->SetTargetMaxDistance(TargetMaxDistance);
		GroundPickTargetActor->SetTargetAreaRadius(TargetingAreaRadius);
		GroundPickTargetActor->AddTargetingAttitude(ETeamAttitude::Hostile);
	}

	WaitTargetDataTask->FinishSpawningActor(this, TargetActor);
}

void UGA_GroundBlast::TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	for (const AActor* Target : UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle))
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Target: %s"), *(Target->GetName()))
	}
	UE_LOG(LogTemp, Warning, TEXT("Target Receieved"))
	K2_EndAbility();
}

void UGA_GroundBlast::TargetCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	UE_LOG(LogTemp, Warning, TEXT("Target Cancelled"))
	K2_EndAbility();
}
