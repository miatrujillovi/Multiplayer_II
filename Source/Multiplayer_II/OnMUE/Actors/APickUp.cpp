// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/BoxComponent.h"
#include "OnMUE/Actors/APickUp.h"
#include "OnMUE/Character/TestCharacter.h"
#include "NET/UnrealNetwork.h"

// Sets default values
AAPickUp::AAPickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	bIsAvailable = true;
}

void AAPickUp::OnRep_SetAmmoParticles(int32 OldAmmo)
{
	FString NewAmmo = HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT");
	//UE_LOG(LogTemp, Warning, TEXT("%s Actor: OLDAMMO: %d | CURRRENT AMMO: "), );
}

// Called when the game starts or when spawned
void AAPickUp::BeginPlay()
{
	Super::BeginPlay();

	//Usamos authority
	if (HasAuthority()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Servidor: Se creo el cartucho %s en el mundo"), *GetName());
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("Cliente: Recibiendo la replicacion del cartucho %s en el mundo"), *GetName());
	}
}

// Called every frame
void AAPickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAPickUp::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAPickUp, bIsAvailable);
}

void AAPickUp::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	ATestCharacter* ACharacter = Cast<ATestCharacter>(OtherActor);

	if (ACharacter && HasAuthority())
	{
		bIsAvailable = false;

		int32 OldAmmo = CurrentAmmo;

		CurrentAmmo = 10;

		SetActorHiddenInGame(true);

		SetActorEnableCollision(false);

		SetLifeSpan(5.0f);

		//Old Value from my replicated property
		OnRep_SetAmmoParticles(OldAmmo);
	}
}



