// Fill out your copyright notice in the Description page of Project Settings.


#include "OnMUE/Abilities/TestAbilities.h"

// Sets default values
ATestAbilities::ATestAbilities()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestAbilities::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestAbilities::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

