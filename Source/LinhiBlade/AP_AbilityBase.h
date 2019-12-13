// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AP_AbilityBase.generated.h"

#define RAY_LENGTH 999999.0f
UENUM(BlueprintType)
enum class ETargetingPolicy :uint8
{
	None = 0,
	Ground = 1,
	UnitAll = 2,
	UnitAlly = 3,
	UnitAllyExcludeSelf = 4,
	UnitEnemy = 5
};

UENUM(BlueprintType)
enum class EActivatingPolicy :uint8
{
	PassivePermanent = 0,
	PassiveTrigger = 1,
	Toggle = 2,
	Active = 3
};

/**
 * 
 */
UCLASS(BlueprintType)
class LINHIBLADE_API UAP_AbilityBase : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UAP_AbilityBase();
	virtual ~UAP_AbilityBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETargetingPolicy TargetingPolicy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EActivatingPolicy ActivatingPolicy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Range;
public:
	UFUNCTION(BlueprintCallable)
		ECollisionChannel GetTraceChannel();
	UFUNCTION(BlueprintCallable)
		bool LineTraceUnit(FVector Start, FVector Direction, AActor*& OutActor);
	UFUNCTION(BlueprintCallable)
		bool LineTraceGround(FVector Start, FVector Direction, FVector& OutLocation);
};