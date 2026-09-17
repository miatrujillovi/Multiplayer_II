// Fill out your copyright notice in the Description page of Project Settings.


#include "OnMUE/Online/MultiplayerGameState.h"

void AMultiplayerGameState::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("GameState::BeginPlay"))
}
