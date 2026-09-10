// Fill out your copyright notice in the Description page of Project Settings.


#include "OnMUE/Character/UOnMUEHealthComponent.h"
#include "UOnMUEHealthComponent.h"

// Sets default values for this component's properties
UUOnMUEHealthComponent::UUOnMUEHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UUOnMUEHealthComponent::ReceivedDeathVisuals()
{
	OnDeath.Broadcast();
}


// Called when the game starts
void UUOnMUEHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	ReceivedDeathVisuals();
	
}


// Called every frame
void UUOnMUEHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ReceivedDeathVisuals();

	// ...
}

