// Fill out your copyright notice in the Description page of Project Settings.


#include "OnMUE/Online/MultiplayerPlayerState.h"

#include "OnMUE/Data/UOnlineAttributeSet.h"

AMultiplayerPlayerState::AMultiplayerPlayerState()
{
	//Replicar el player state
	bReplicates = true;
	SetNetUpdateFrequency(100.0f);
	
	//Create subject Ability system component
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	
	//Create Subobject
	AttributeSet = CreateDefaultSubobject<UUOnlineAttributeSet>("OnlineAttributeSet");
	
	//Settings
	AbilitySystemComponent->SetIsReplicated(true);
	
	//Get a replicated mode for human players
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
}

UAbilitySystemComponent* AMultiplayerPlayerState::GetAbilitySystemComponent() const
{
	
	
	return AbilitySystemComponent;
}
