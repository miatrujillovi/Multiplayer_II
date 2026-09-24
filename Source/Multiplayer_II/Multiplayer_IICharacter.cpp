// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer_IICharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerState.h"
#include "Multiplayer_II.h"
#include <Multiplayer_IIPlayerController.h>

namespace
{
	const TCHAR* NetModeToString(const ENetMode NetMode)
	{
		switch (NetMode)
		{
		case NM_Standalone:
			return TEXT("Standalone");


		case NM_DedicatedServer:
			return TEXT("DedicatedServer");


		case NM_ListenServer:
			return TEXT("ListenServer");


		case NM_Client:
			return TEXT("Client");


		default:
			return TEXT("Unknown");
		}
	}
}

AMultiplayer_IICharacter::AMultiplayer_IICharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
}

void AMultiplayer_IICharacter::BeginPlay()
{
	Super::BeginPlay();


	const ENetMode NetMode = GetNetMode();


	const FString Label = FString::Printf(
		TEXT("%s | %s | Local=%s"),
		NetModeToString(NetMode),
		*UEnum::GetValueAsString(GetLocalRole()),
		IsLocallyControlled() ? TEXT("true") : TEXT("false")
	);


	if (NetMode != NM_DedicatedServer)
	{
		DrawDebugString(
			GetWorld(),
			FVector(0.0, 0.0, 120.0),
			Label,
			this,
			IsLocallyControlled() ? FColor::Green : FColor::Cyan,
			160.0f,
			true
		);
	}
}

void AMultiplayer_IICharacter::OnRep_PlayerState() 
{
	Super::OnRep_PlayerState();

	APlayerState* PS = GetPlayerState();
	if (PS && IsLocallyControlled()) 
	{
		CurrentPS = PS;
		UE_LOG(LogTemp, Display, TEXT("Player: %d, Score: %f"), CurrentPS->GetPlayerId(), CurrentPS->GetScore());
	}
}

void AMultiplayer_IICharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	CurrentPS = GetPlayerState();

	if (CurrentPS && IsLocallyControlled())
	{
		UE_LOG(LogTemp, Display, TEXT("Player: %d, Score: %f"), CurrentPS->GetPlayerId(), CurrentPS->GetScore());
	}
}

void AMultiplayer_IICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMultiplayer_IICharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMultiplayer_IICharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMultiplayer_IICharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMultiplayer_IICharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AMultiplayer_IICharacter::LookInput);
	}
	else
	{
		UE_LOG(LogMultiplayer_II, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AMultiplayer_IICharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void AMultiplayer_IICharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void AMultiplayer_IICharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AMultiplayer_IICharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AMultiplayer_IICharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void AMultiplayer_IICharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

void AMultiplayer_IICharacter::DoDestroy()
{
	if (!IsLocallyControlled()) return;

	//if (CurrentPS)
	//{
		CurrentPS->SetScore(CurrentPS->GetScore() + 1);

		UE_LOG(LogTemp, Display, TEXT("Player: %d, Score: %f"), CurrentPS->GetPlayerId(), CurrentPS->GetScore());
	//}

	AMultiplayer_IIPlayerController* PlayerController = Cast<AMultiplayer_IIPlayerController>(GetController());
	PlayerController->Server_SolicitarRespawn();

	//Destroy();
}
