// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer_IICharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"

#include "AbilitySystemComponent.h"
#include "OnMUE/Online/MultiplayerPlayerState.h"
#include "GameplayTagsManager.h"
#include "OnMUE/Data/UOnlineAttributeSet.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerState.h"
#include "Multiplayer_II.h"
#include "OnMUE/Actors/OnMueWC.h"
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
	
	// Configure the WeaponComponent
	WeaponComponent = CreateDefaultSubobject<UOnMUEWC>(TEXT("WeaponComponent"));
	
	// Get the saved tag in DefaultGameplayTag.ini
	SprintingTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("State.Movement.Sprinting"));
	StunnedTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("State.Debug.Stunned"));
}

FVector AMultiplayer_IICharacter::GetAimOrigin() const
{
	return GetPawnViewLocation(); //Returns the pawn location
}

FVector AMultiplayer_IICharacter::GetAimDirection() const
{
	return GetViewRotation().Vector(); //Returns the pawn rotation
}

UAbilitySystemComponent* AMultiplayer_IICharacter::GetAbilitySystemComponent() const
{
	AMultiplayerPlayerState* PS = GetPlayerState<AMultiplayerPlayerState>();
	
	if (PS)
	{
		return PS->GetAbilitySystemComponent();
	}
	
	return nullptr;
}

void AMultiplayer_IICharacter::BroadcastInitialAttributes()
{
	if (AMultiplayerPlayerState* PS = GetPlayerState<AMultiplayerPlayerState>())
	{
		if (UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent())
		{
			const float CurrentHealth = ASC->GetNumericAttribute(UUOnlineAttributeSet::GetHealthAttribute());
			const float CurrentMaxHealth = ASC->GetNumericAttribute(UUOnlineAttributeSet::GetMaxHealthAttribute());
			const float CurrentArmor = ASC->GetNumericAttribute(UUOnlineAttributeSet::GetArmorAttribute());
			
			OnHealthChanged.Broadcast(CurrentHealth, CurrentMaxHealth);
			OnArmorChanged.Broadcast(CurrentArmor, 100.0f);
		}
	}
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

	InitAbilityActorInfo();
	
	BroadcastInitialAttributes();
}

void AMultiplayer_IICharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
	
	BroadcastInitialAttributes();
}

void AMultiplayer_IICharacter::InitAbilityActorInfo()
{
	AMultiplayerPlayerState* OnlinePS = GetPlayerState<AMultiplayerPlayerState>();
	
	if (OnlinePS)
	{
		//ASC
		UAbilitySystemComponent* ASC = OnlinePS->GetAbilitySystemComponent();
		if (ASC)
		{
			//Get Owner and Avatar
			ASC->InitAbilityActorInfo(OnlinePS, this);
			BindAttributeCallbacks(ASC);
			
			//-------------------------
			if (HasAuthority() || IsValid(ASC))
			{
				for(TSubclassOf<UGameplayAbility> AbilityClass: DefaultAbilities)
				{
					if (AbilityClass)
					{
						FGameplayAbilitySpec AbilitySpec(AbilityClass, 1, INDEX_NONE, this);
						ASC->GiveAbility(AbilitySpec);
					}
				}
			}
			
			UAbilitySystemComponent* AbilitySystemComponent = ASC;
			//AbilitySystemComponent = ASC;
		}
	}
}

void AMultiplayer_IICharacter::ToggleStun(bool bIsStunned)
{
	if (AMultiplayerPlayerState* OnlinePS = GetPlayerState<AMultiplayerPlayerState>())
	{
		if (UAbilitySystemComponent* ASC = OnlinePS->GetAbilitySystemComponent())
		{
			if (bIsStunned)
			{
				if (!ASC->HasMatchingGameplayTag(StunnedTag))
				{
					ASC->AddLooseGameplayTag(StunnedTag);
				}
			}
			else
			{
				ASC->RemoveLooseGameplayTag(StunnedTag);
			}
		}
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
		
		//Fire
		EnhancedInputComponent->BindAction(FireInput, ETriggerEvent::Triggered, this, &AMultiplayer_IICharacter::DoFire);
		
		//Sprint
		EnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Triggered, this, &AMultiplayer_IICharacter::StartSprinting);
		EnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Completed, this, &AMultiplayer_IICharacter::StopSprinting);
		
		//Melee
		EnhancedInputComponent->BindAction(MeleeAction, ETriggerEvent::Triggered, this, &AMultiplayer_IICharacter::DoMelee);
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

void AMultiplayer_IICharacter::DoFire()
{
	if (WeaponComponent)
	{
		WeaponComponent->TryStartFire();
	}
}

void AMultiplayer_IICharacter::StartSprinting()
{
	if (AMultiplayerPlayerState* OnlinePS = GetPlayerState<AMultiplayerPlayerState>())
	{
		if (UAbilitySystemComponent* ASC = OnlinePS->GetAbilitySystemComponent())
		{
			// Ask if the player is stunned
			if (ASC->HasMatchingGameplayTag(StunnedTag))
			{
				return;
			}
			FGameplayTag ActionSprintTag = FGameplayTag::RequestGameplayTag(FName("Ability.Action.Sprint"));
			
			// The client ask if the activate ability with the corresponding tag
			ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(ActionSprintTag));
		}
	}
}

void AMultiplayer_IICharacter::StopSprinting()
{
	if (AMultiplayerPlayerState* OnlinePS = GetPlayerState<AMultiplayerPlayerState>())
	{
		if (UAbilitySystemComponent* ASC = OnlinePS->GetAbilitySystemComponent())
		{
			FGameplayTag ActionSprintTag = FGameplayTag::RequestGameplayTag(FName("Ability.Action.Sprint"));
			FGameplayTagContainer SprintTags(ActionSprintTag);
			
			//Cancel the ability
			ASC->CancelAbilities(&SprintTags);
		}
	}
}

void AMultiplayer_IICharacter::DoMelee()
{
	if (AMultiplayerPlayerState* OnlinePS = GetPlayerState<AMultiplayerPlayerState>())
	{
		if (UAbilitySystemComponent* ASC = OnlinePS->GetAbilitySystemComponent())
		{
			// Ask if the player is stunned
			if (ASC->HasMatchingGameplayTag(StunnedTag))
			{
				return;
			}
			FGameplayTag ActionMeleeTag = FGameplayTag::RequestGameplayTag(FName("Ability.Action.Melee"));
			
			// The client ask if the activate ability with the corresponding tag
			ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(ActionMeleeTag));
		}
	}
}

void AMultiplayer_IICharacter::BindAttributeCallbacks(UAbilitySystemComponent* ASC)
{
	if (!ASC) return;
	
	// Remove previous bindings
	if (HealthChangedDelegateHandle.IsValid())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UUOnlineAttributeSet::GetHealthAttribute()).Remove(HealthChangedDelegateHandle);
	}

	if (ArmorChangedDelegateHandle.IsValid())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UUOnlineAttributeSet::GetArmorAttribute()).Remove(ArmorChangedDelegateHandle);
	}

	// Bind Health
	HealthChangedDelegateHandle =
		ASC->GetGameplayAttributeValueChangeDelegate(UUOnlineAttributeSet::GetHealthAttribute()).AddLambda([this, ASC](const FOnAttributeChangeData& Data)
	{
		const float MaxHealth = ASC->GetNumericAttribute(UUOnlineAttributeSet::GetMaxHealthAttribute());

		OnHealthChanged.Broadcast(Data.NewValue, MaxHealth);
	});

	// Bind Armor
	ArmorChangedDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(UUOnlineAttributeSet::GetArmorAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		OnArmorChanged.Broadcast(Data.NewValue, 100.0f);
	});
	
	//On Value Changed Health
	/*ASC->GetGameplayAttributeValueChangeDelegate(UUOnlineAttributeSet::GetHealthAttribute()).AddLambda([this, ASC](const FOnAttributeChangeData& Data)
	{
		const float MaxHealth = ASC->GetNumericAttribute(UUOnlineAttributeSet::GetMaxHealthAttribute());
		OnHealthChanged.Broadcast(Data.NewValue, MaxHealth);
	});
	
	//On Value Changed Armor
	ASC->GetGameplayAttributeValueChangeDelegate(UUOnlineAttributeSet::GetArmorAttribute()).AddLambda([this, ASC](const FOnAttributeChangeData& Data)
	{
		OnArmorChanged.Broadcast(Data.NewValue, 100.0f);
	});*/
}

/*bool AMultiplayer_IICharacter::HasStateTag(FGameplayTag TagToCheck) const
{
	if (AMultiplayerPlayerState* OnlinePS = GetPlayerState<AMultiplayerPlayerState>())
	{
		if (UAbilitySystemComponent* ASC = OnlinePS->GetAbilitySystemComponent())
		{
			return ASC->HasMatchingGameplayTag(TagToCheck);
		}
		return false;
	}
	return false;
}*/
