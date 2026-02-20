// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_Combo.h"
#include "Ability/CGameplayTypes.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "GameplayTagsManager.h"
#include "AbilitySystemBlueprintLibrary.h"

UGA_Combo::UGA_Combo()
{
	AbilityTags.AddTag(TAG_Combo);
	BlockAbilitiesWithTag.AddTag(TAG_Combo);
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayComboMontageTask = 
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ComboMontage);

		PlayComboMontageTask->OnBlendOut.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayComboMontageTask->OnInterrupted.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayComboMontageTask->OnCancelled.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayComboMontageTask->OnCompleted.AddDynamic(this, &UGA_Combo::K2_EndAbility);

		PlayComboMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitComboChangeEvent = 
			UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TAG_Combo_Change, nullptr, false, false);
		WaitComboChangeEvent->EventReceived.AddDynamic(this, &UGA_Combo::HandleComboChange);
		WaitComboChangeEvent->ReadyForActivation();
	}

	if (K2_HasAuthority())
	{
		UAbilityTask_WaitGameplayEvent* WaitDamageEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TAG_Combo_Damage);
		WaitDamageEvent->EventReceived.AddDynamic(this, &UGA_Combo::HandleDamageEvent);
		WaitDamageEvent->ReadyForActivation();
	}

	BindInputPressedEvent();
}

TSubclassOf<UGameplayEffect> UGA_Combo::GetDamageEffectForCurrentCombo() const
{
	UAnimInstance* OwnerAnimInst = GetOwnerAnimInst();
	if (OwnerAnimInst)
	{
		const TSubclassOf<UGameplayEffect>* FoundDamageEffect =  DamageEffectMap.Find(OwnerAnimInst->Montage_GetCurrentSection(ComboMontage));
		if (FoundDamageEffect)
		{
			return *FoundDamageEffect;
		}
	}

	return DefaultDamageEffect;
}

void UGA_Combo::HandleComboChange(FGameplayEventData EventData)
{
	FGameplayTag EventTag = EventData.EventTag;

	if (EventTag == TAG_Combo_Change_End)
	{
		NextComboName = NAME_None;
		UE_LOG(LogTemp, Warning, TEXT("Combo is cleared"))
		return;
	}

	TArray<FName> TagNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
	NextComboName = TagNames.Last();

	UE_LOG(LogTemp, Warning, TEXT("Next Combo is %s"), *(NextComboName.ToString()))
}

void UGA_Combo::HandleDamageEvent(FGameplayEventData EventData)
{
	//UE_LOG(LogTemp, Warning, TEXT("Doing Damage starting at %s"), *(EventData.TargetData.Get(0)->GetOrigin().GetLocation().ToString()))
	TArray<FHitResult> Targets = GetHitResultFromTargetData(EventData.TargetData, DamageDetectionRadius, true, bShouldDrawDebug);
	for (const FHitResult& Target : Targets)
	{
		FGameplayEffectSpecHandle DamageEffectSpec = 
			MakeOutgoingGameplayEffectSpec(GetDamageEffectForCurrentCombo(), GetAbilityLevel(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()));

		FGameplayEffectContextHandle Context = MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
		Context.AddHitResult(Target);

		DamageEffectSpec.Data->SetContext(Context);

		ApplyGameplayEffectSpecToTarget(
			GetCurrentAbilitySpecHandle(),
			GetCurrentActorInfo(),
			GetCurrentActivationInfo(),
			DamageEffectSpec,
			UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(Target)
		);

		/*ApplyGameplayEffectToTarget(
			GetCurrentAbilitySpecHandle(),
			GetCurrentActorInfo(),
			GetCurrentActivationInfo(),
			UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(Target),
			GetDamageEffectForCurrentCombo(),
			GetAbilityLevel(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo())
		);*/
	}
}

void UGA_Combo::BindInputPressedEvent()
{
	UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this, false);
	WaitInputPress->OnPress.AddDynamic(this, &UGA_Combo::HandleComboChangeCommit);
	WaitInputPress->ReadyForActivation();
}

void UGA_Combo::HandleComboChangeCommit(float TimeWaited)
{
	BindInputPressedEvent();

	if (NextComboName == NAME_None) 
	{
		return;
	}

	if (UAnimInstance* OwnerAnimInstance = GetOwnerAnimInst())
	{
		OwnerAnimInstance->Montage_SetNextSection(OwnerAnimInstance->Montage_GetCurrentSection(ComboMontage), NextComboName, ComboMontage);
	}
}
