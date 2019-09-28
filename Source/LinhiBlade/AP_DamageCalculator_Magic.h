// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AP_DamageCalculator_Magic.generated.h"

/**
 * 
 */
UCLASS()
class LINHIBLADE_API UAP_DamageCalculator_Magic : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UAP_DamageCalculator_Magic();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
