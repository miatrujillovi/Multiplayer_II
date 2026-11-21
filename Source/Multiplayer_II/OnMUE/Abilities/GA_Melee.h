// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Melee.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_II_API UGA_Melee : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Melee();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;
	
	protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* MeleeMontage;
	
	//Gameplay Task's Callbacks
	UFUNCTION()
	void OnMontageCompleted();
	
	UFUNCTION()
	void OnMontageInterrupted();
	
	UFUNCTION()
	void OnEventReceived(FGameplayEventData Payload);
	
};
