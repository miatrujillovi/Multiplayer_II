// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UOnMUEHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDamage);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDamageReceivedVisuals);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYER_II_API UUOnMUEHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUOnMUEHealthComponent();

	/*UFUNCTION(BlueprintCallable, Category = "OnDie")
	void ReceivedDeathVisuals();

	UPROPERTY(BlueprintAssignable, Category = "OnDie")
	FOnDamageReceivedVisuals OnDeath;*/

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float maxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float currentHealth;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDamage OnDamage;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void OnDamageReceived();
};
