// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AP_DamageCalculator_Physic.generated.h"

/**
 * 
 */
UCLASS()
class LINHIBLADE_API UAP_DamageCalculator_Physic : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UAP_DamageCalculator_Physic();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
