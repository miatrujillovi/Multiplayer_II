// Fill out your copyright notice in the Description page of Project Settings.


#include "OnMUE/Abilities/GA_Melee.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"

UGA_Melee::UGA_Melee()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	// Add the state tag automatically
	FGameplayTag MeleeTag = FGameplayTag::RequestGameplayTag(FName("State.Combat.InCombat"));
	ActivationOwnedTags.AddTag(MeleeTag);
}

void UGA_Melee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	//1. Wait for the impact event (WaitGameplay Event)
	UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, 
		FGameplayTag::RequestGameplayTag(FName("Event.Montage.MeleeHit")), nullptr, false, false);
	
	if (WaitEventTask)
	{
		WaitEventTask->EventReceived.AddDynamic(this, &UGA_Melee::OnEventReceived);
		WaitEventTask->ReadyForActivation();
	}
	
	//2. Task: Play Animation Montage and Wait
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, 
		NAME_None, MeleeMontage, 1.0f);
	
	if (PlayMontageTask)
	{
		PlayMontageTask->OnCompleted.AddDynamic(this, &UGA_Melee::OnMontageCompleted);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UGA_Melee::OnMontageInterrupted);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &UGA_Melee::OnMontageInterrupted);
		PlayMontageTask->ReadyForActivation();
	}
}

void UGA_Melee::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Melee::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_Melee::OnEventReceived(FGameplayEventData Payload)
{
	//Apply the Damage Effect
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Melee::OnEventReceived"));
}
