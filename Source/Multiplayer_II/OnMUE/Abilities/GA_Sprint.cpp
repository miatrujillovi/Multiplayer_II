// Fill out your copyright notice in the Description page of Project Settings.


#include "OnMUE/Abilities/GA_Sprint.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_Sprint::UGA_Sprint()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	// Add the state tag automatically
	FGameplayTag SprintTag = FGameplayTag::RequestGameplayTag(FName("State.Movement.Sprinting"));
	ActivationOwnedTags.AddTag(SprintTag);
}

void UGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = 900.0f;
	}
}

void UGA_Sprint::EndAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCanceled)
{
	Super::EndAbility(SpecHandle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCanceled);
	
	const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
}

bool UGA_Sprint::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
                                    FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character || Character->GetCharacterMovement()->IsFalling())
	{
		return false;
	}
	return true;
}
