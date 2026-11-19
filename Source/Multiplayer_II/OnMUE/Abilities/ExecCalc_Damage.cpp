// Fill out your copyright notice in the Description page of Project Settings.


#include "OnMUE/Abilities/ExecCalc_Damage.h"
#include "OnMUE/Data/UOnlineAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Curves/BezierUtilities.h"

struct FDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	
	FDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UUOnlineAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UUOnlineAttributeSet, Health, Target, false);
	}
};

static const FDamageStatics& DamageStatics()
{
	static FDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	//Add the defined attributes from the list
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	AActor* Source = SourceASC ? SourceASC->GetAvatarActor() : nullptr;

	AActor* Target = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedSourceTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;

	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// 1 capture the actual value of the armor
	float Armor =  0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().ArmorDef,
		EvaluationParameters,
		Armor);

	Armor = FMath::Max(Armor, 0.0f);

	//2. Extraer el  da;o base pasado mediante SetByCallerMagnitude
	float BaseDamage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), false, 0.0f);

	//3. Aplicar la formula matematica de damage
	float ArmorMitigation = 100.f / (100.0f + Armor);
	float FinalDamage = BaseDamage * ArmorMitigation;

	//More Conditional logic (Verify the tags)

	if (TargetTags && TargetTags->HasTag(FGameplayTag::RequestGameplayTag(FName("State.Buff.Shield"))))
	{
		FinalDamage *= 0.5f;//Mitigate aditional damagae by 50% if target has Shield
	}
	if (FinalDamage > 0.0f)
	{
		//Modificar el atribute Health del Target de forma negativa
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().HealthProperty, EGameplayModOp::Additive, -FinalDamage));
	}
}
