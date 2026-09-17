// Fill out your copyright notice in the Description page of Project Settings.


#include "OnMUE/Online/MultiplayerGameMode.h"

void AMultiplayerGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("GameMode::BeginPlay"))
}
