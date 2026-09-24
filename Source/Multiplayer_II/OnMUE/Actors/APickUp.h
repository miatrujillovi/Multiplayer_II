// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "APickUp.generated.h"

UCLASS()
class MULTIPLAYER_II_API AAPickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAPickUp();

	UPROPERTY(ReplicatedUsing = OnRep_IsAvailable, BlueprintReadOnly, Category = "Replicated")
	bool bIsAvailable;

	UFUNCTION()
	void OnRep_IsAvailable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void PickUpAmmo();
};
