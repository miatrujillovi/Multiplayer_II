// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MultiplayerGameState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_II_API AMultiplayerGameState : public AGameStateBase
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
};
