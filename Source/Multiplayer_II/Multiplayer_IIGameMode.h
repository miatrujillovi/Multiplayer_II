// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Multiplayer_IIGameMode.generated.h"

/**
 *  Simple GameMode for a first person game
 */
UCLASS(abstract)
class AMultiplayer_IIGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMultiplayer_IIGameMode();
};



