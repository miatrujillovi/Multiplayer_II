// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"

#include "OnMUE/Actors/OnMUEWC.h"
#include "../Data/IAimSource.h"
#include "../Data/WeaponData.h"

// Sets default values for this component's properties
UOnMUEWC::UOnMUEWC()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UOnMUEWC::TryStartFire()
{
	AActor* OwnerActor = GetOwner();
	
	//1. Initial Validations: Authority and Data Asset
	//This logic must be executed in server
	if (!OwnerActor || !OwnerActor->HasAuthority() || !WeaponDefinition) return;
	
	//2. Determinate the origin and direction of the weapon using IAimSource
	FVector Origin;
	FVector Direction;
	if (IAimSource* AimInterface = Cast<IAimSource>(OwnerActor))
	{
		Origin = AimInterface->GetAimOrigin();
		Direction = AimInterface->GetAimDirection();
	} 
	else
	{
		//Security fallback if the owner has no interface
		FRotator ViewRotation;
		OwnerActor->GetActorEyesViewPoint(Origin, ViewRotation);
		Direction = ViewRotation.Vector();
	}
	
	FVector EndLocation = Origin + (Direction * 1000);
	
	//3. Line trace execution
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerActor); //Ignore yourself
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Origin, EndLocation, ::ECC_Visibility, QueryParams);
	
	DrawDebugLine(GetWorld(), Origin, EndLocation, bHit ? FColor::Green : FColor::Red, false, 2.0f, 0, 1.5f);

	if (bHit)
	{
		DrawDebugBox(GetWorld(), HitResult.ImpactPoint, FVector(4.0f, 4.0f, 4.0f), FColor::Yellow, false, 2.0f, 0, 1.5f);
	}
	
	if (bHit && HitResult.GetActor())
	{
		AActor* TargetActor = HitResult.GetActor();
		
		//Get both ASC
		UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		
		if (!SourceASC)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ERROR: SourceASC es NULL (Revisa la compilación de la Interfaz del atacante)"));
		}
		if (!TargetASC)
		{
			FString ObjectiveName = TargetActor ? TargetActor->GetName() : TEXT("Nada");
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, (TEXT("ERROR: TargetASC es NULL. Le disparaste a: %s"), *ObjectiveName));
		}
		if (WeaponDefinition && !WeaponDefinition->DamageEffect)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ERROR: DamageEffect is NULL. Falta asignar el GE en el Data Asset"));
		}
		
		//Validate both ASC and if the weapon data has a valid effect
		if (SourceASC && TargetASC)
		{
			//Create the context effect
			FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);
			
			//KEY POINT. Send HitResult to the context
			//This allows ExecCal for headshots
			EffectContext.AddHitResult(HitResult);
			
			//Create the SpecHandle
			FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(WeaponDefinition->DamageEffect, 1.0f, EffectContext);
			if (SpecHandle.IsValid())
			{
				SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), WeaponDefinition->BaseDamage);
				
				//Apply the effect to the objective
				SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
			}
		}
	}
}

// Called when the game starts
void UOnMUEWC::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UOnMUEWC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

