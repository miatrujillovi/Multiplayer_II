// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "OnMUE/Data/IAimSource.h"
#include "AbilitySystemInterface.h"
#include "Multiplayer_IICharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeChangedSignature, float, NewValue, float, MaxValue);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class AMultiplayer_IICharacter : public ACharacter, public IAimSource, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* FireInput;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* SprintInputAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* MeleeAction;
	
public:
	AMultiplayer_IICharacter();

	APlayerState* CurrentPS;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class UOnMUEWC* WeaponComponent;
	
	virtual FVector GetAimOrigin() const override;
	
	virtual FVector GetAimDirection() const override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;
	
	UPROPERTY(BlueprintAssignable, Category = "GAS|UI")
	FOnAttributeChangedSignature OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "GAS|UI")
	FOnAttributeChangedSignature OnArmorChanged;
	
	UFUNCTION(BlueprintCallable, Category = "GAS|UI")
	void BroadcastInitialAttributes();

protected:

	//BEGIN PLAY
	virtual void BeginPlay() override;

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoFire();
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void StartSprinting();
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void StopSprinting();
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoMelee();
	
	/*UFUNCTION(BlueprintCallable, Category = "Input")
	bool HasStateTag(FGameplayTag TagToCheck) const;*/
	
	void BindAttributeCallbacks(UAbilitySystemComponent* ASC);

protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	

public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	virtual void OnRep_PlayerState();

	virtual void PossessedBy(AController* NewController) override;

	void InitAbilityActorInfo();
	
	UFUNCTION(BlueprintCallable, Category = "State")
	void ToggleStun(bool bIsStunned);
	
private:
	//Tag Cache
	FGameplayTag SprintingTag;
	FGameplayTag StunnedTag;
	
	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle ArmorChangedDelegateHandle;
};

