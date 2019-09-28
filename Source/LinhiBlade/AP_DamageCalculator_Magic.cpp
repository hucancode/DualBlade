// Fill out your copyright notice in the Description page of Project Settings.

#include "AP_DamageCalculator_Magic.h"
#include "AP_AttributeSet.h"
#include <GameplayAbilities\Public\AbilitySystemBlueprintLibrary.h>

struct AP_DamageContext_Magic
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Evasion);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicalPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);

	AP_DamageContext_Magic()
	{
		// Capture the Target's DefensePower attribute. Do not snapshot it, because we want to use the health value at the moment we apply the execution.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAP_AttributeSet, Evasion, Target, false);
		// Also capture the source's raw Damage, which is normally passed in directly via the execution
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAP_AttributeSet, Health, Target, false);

		// Capture the Source's AttackPower. We do want to snapshot this at the moment we create the GameplayEffectSpec that will execute the damage.
		// (imagine we fire a projectile: we create the GE Spec when the projectile is fired. When it hits the target, we want to use the AttackPower at the moment
		// the projectile was launched, not when it hits).
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAP_AttributeSet, MagicalPower, Source, true);
	}
};

static const AP_DamageContext_Magic& DamageContextMagic()
{
	static AP_DamageContext_Magic ret;
	return ret;
}
UAP_DamageCalculator_Magic::UAP_DamageCalculator_Magic()
{
	RelevantAttributesToCapture.Add(DamageContextMagic().EvasionDef);
	RelevantAttributesToCapture.Add(DamageContextMagic().HealthDef);
	RelevantAttributesToCapture.Add(DamageContextMagic().MagicalPowerDef);
}

void UAP_DamageCalculator_Magic::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->AvatarActor : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->AvatarActor : nullptr;
	

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// the real calculation start here

	float Evasion = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageContextMagic().EvasionDef, EvaluationParameters, Evasion);
	float Damage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageContextMagic().MagicalPowerDef, EvaluationParameters, Damage);
	bool Hit = FMath::RandRange(0.0f, 1.0f) > Evasion;
	float DamageDone = Hit ? FMath::Clamp(Damage, 0.0f, 99999.0f) : 0.0f;
	UE_LOG(LogTemp, Warning, TEXT("calculating damage, damage done = %f"), DamageDone);
	if (FMath::Abs(DamageDone) > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageContextMagic().HealthProperty, EGameplayModOp::Additive, -DamageDone));
	}
	FGameplayEventData DamageData;
	DamageData.Instigator = SourceActor;
	DamageData.Target = TargetActor;
	DamageData.EventMagnitude = DamageDone;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SourceActor,
		FGameplayTag::RequestGameplayTag("Combat.DamageCaused"), DamageData);
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor,
		FGameplayTag::RequestGameplayTag("Combat.DamageReceived"), DamageData);
}