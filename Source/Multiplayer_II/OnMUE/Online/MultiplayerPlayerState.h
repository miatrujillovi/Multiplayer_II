// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

//GAS - Gameplay Ability System
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

#include "MultiplayerPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_II_API AMultiplayerPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AMultiplayerPlayerState();
	
	//Implementamos la interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS | Components")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS | Attributes")
	TObjectPtr<class UUOnlineAttributeSet> AttributeSet;
};
