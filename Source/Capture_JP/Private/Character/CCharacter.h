// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"

#include "GenericTeamAgentInterface.h"

#include "AbilitySystemInterface.h"
#include "CCharacter.generated.h"

UCLASS()
class ACCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACCharacter();

	void ServerSideInit();
	void ClientSideInit();

	/* Returns properties */
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/************************************/
	/*			Gameplay Ability		*/
	/************************************/

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Ability")
	class UCAbilitySystemComponent* CAbilitySystemComponent;

	UPROPERTY()
	class UCAttributeSet* CAttributeSet;

	void BindGameplayTagChangeEvents();

	void DeathTagChanged(const FGameplayTag Tag, int32 NewCount);
	void StunTagChanged(const FGameplayTag Tag, int32 NewCount);
	void AimTagChanged(const FGameplayTag Tag, int32 NewCount);
	virtual void AimStateChanged(bool bNewIsAiming);

	/************************************/
	/*			Death and Respawn		*/
	/************************************/
public:

	bool IsCharacterDead() const;

private:
	void StartDeathSequence();
	void Respawn();
	void PlayDeathMontage();

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	UAnimMontage* DeathMontage;

	virtual void OnDead() {}
	virtual void OnRespawn() {}

	void SetEnableRagdoll(bool bEnableRagdoll);

	FTransform MeshRelativeTransform;
	FTimerHandle DeathAnimationFinishedTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float DeathAnimationDurationOffset = -0.5f;

	void DeathAnimationFinished();

	/************************************/
	/*			Stunning				*/
	/************************************/
private:
	virtual void OnStunStatChanged(bool bNewIsStunned);

	UPROPERTY(EditDefaultsOnly, Category = "Stun")
	UAnimMontage* StunMontage;

	/************************************/
	/*				UI					*/
	/************************************/
private:
	UPROPERTY(VisibleAnywhere, Category = "Widgets")
	class UWidgetComponent* OverheadWidgetComponent;

	void InitializeOverheadWidget();

	void SetOverheadWidgetEnabled(bool bWidgetEnabled);

	/************************************/
	/*				Team				*/
	/************************************/

public:

	/** Assigns Team Agent to given TeamID */
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	/** Retrieve team identifier in form of FGenericTeamId */
	FORCEINLINE virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;

	/************************************/
	/*				Perception			*/
	/************************************/

private:
	UPROPERTY()
	class UAIPerceptionStimuliSourceComponent* PerceptionStimulusComponent;

	void SetPerceptionStimuliEnabled(bool bStimuliEnabled);
};
