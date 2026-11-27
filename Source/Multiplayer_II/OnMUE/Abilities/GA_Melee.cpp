// Fill out your copyright notice in the Description page of Project Settings.


#include "OnMUE/Abilities/GA_Melee.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "DrawDebugHelpers.h"
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
	//Validate Authority
	if (!HasAuthority(&CurrentActivationInfo)) return;
	
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	
	if (!AvatarActor || !SourceASC || !DamageEffectClass) return;
	
	//Define the origin and direction of our hit
	FVector StartLocation;
	FRotator ViewRotation;
	
	AvatarActor->GetActorEyesViewPoint(StartLocation, ViewRotation);
	FVector EndLocation = StartLocation + (ViewRotation.Vector() * MeleeRange);
	
	//Execute physic collision
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);
	
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(MeleeRadius),
		QueryParams);
	
	//Debug Melee Sphere
	DrawDebugSphere(GetWorld(), 
					bHit ? HitResult.ImpactPoint : EndLocation, 
					MeleeRadius, 
					12, 
					bHit ? FColor::Green : FColor::Red, 
					false, 
					1.5f);
	
	if (bHit && HitResult.GetActor())
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor());
		if (TargetASC)
		{
			FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
			EffectContext.AddSourceObject(AvatarActor);
			EffectContext.AddHitResult(HitResult);
			
			FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContext);
			if (SpecHandle.IsValid())
			{
				SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), MeleeRange);
			}
		}
	}
}
