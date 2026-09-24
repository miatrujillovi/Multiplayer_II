// Fill out your copyright notice in the Description page of Project Settings.


#include "OnMUE/Actors/APickUp.h"

// Sets default values
AAPickUp::AAPickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	bIsAvailable = true;
}

void AAPickUp::OnRep_IsAvailable()
{
	UStaticMeshComponent* Mesh = FindComponentByClass<UStaticMeshComponent>();

	if (Mesh)
	{
		Mesh->SetVisibility(bIsAvailable);
	}
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

void AAPickUp::PickUpAmmo()
{
	if (HasAuthority()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Se ha consumido el objeto"));

		bIsAvailable = false;

		//Update the server
		OnRep_IsAvailable();
	}
}



