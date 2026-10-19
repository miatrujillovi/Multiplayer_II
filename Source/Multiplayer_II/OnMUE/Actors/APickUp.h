// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "APickUp.generated.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "APickUp.generated.h"

UCLASS()
class MULTIPLAYER_II_API AAPickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAPickUp();

	//Set a collision component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* BoxCollision;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Replicated")
	bool bIsAvailable;

	UPROPERTY(ReplicatedUsing = OnRep_SetAmmoParticles, BlueprintReadOnly, Category = "Replicated")
	int32 CurrentAmmo = 0;

	UFUNCTION()
	void OnRep_SetAmmoParticles(int32 OldAmmo);

	/*UFUNCTION()
	void OnRep_IsAvailable();*/

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
