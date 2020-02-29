// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTargetDataFilter.h"
#include "TeamAgent.h"
#include "AP_AbilityBase.generated.h"

UENUM(BlueprintType)
enum class ETargetingPolicy :uint8
{
	None = 0,
	Ground = 1,
	UnitAll = 2,
	UnitExcludeSelf = 3,
	UnitAlly = 4,
	UnitAllyExcludeSelf = 5,
	UnitEnemy = 6
};

UENUM(BlueprintType)
enum class EActivatingPolicy :uint8
{
	PassivePermanent = 0,
	PassiveOnEvent = 1,
	Toggle = 2,
	Active = 3,
	ActiveAutocast = 4,
	ActiveCombo = 5
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
	virtual bool ShouldAbilityRespondToEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData* Payload) const;
	//virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETargetingPolicy TargetingPolicy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EActivatingPolicy ActivatingPolicy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Range;
public:
	UFUNCTION(BlueprintPure, Category = Ability)
		FGameplayAbilityTargetingLocationInfo FrontOfOwner(float Distance);
	UFUNCTION(BlueprintPure, Category = Ability)
		float OwnerRotationZ();
	UFUNCTION(BlueprintPure, Category = Ability)
		FGameplayAbilityTargetingLocationInfo FrontOfOwnerTilted(float Distance, float Angle);
	UFUNCTION(BlueprintPure, Category = Ability)
		FGameplayTargetDataFilterHandle FilterSelfOut();
	UFUNCTION(BlueprintPure, Category = Ability)
		ECollisionChannel GetEnemyCollisionChannel();
	UFUNCTION(BlueprintCallable, Category = Ability)
		void SetOwnerMovementMode(EMovementMode NewMode);
	UFUNCTION(BlueprintCallable, Category = Ability)
		void SetOwnerMovementWalk();
	UFUNCTION(BlueprintCallable, Category = Ability)
		void SetOwnerMovementFly();
	UFUNCTION(BlueprintPure, Category = Ability)
		bool IsOwnerMovingFoward(float AngleThreshold = 15.0f);
	UFUNCTION(BlueprintPure, Category = Ability)
		bool IsOwnerInAir();
	UFUNCTION(BlueprintPure, Category = Ability)
		bool IsBuffApplied(TSubclassOf<UGameplayEffect> BuffClass);
	UFUNCTION(BlueprintCallable, Category = Ability)
		void ClearBuff(TSubclassOf<UGameplayEffect> BuffClass);
	UFUNCTION(BlueprintCallable, Category = Ability)
		void EnterCloak(ECloakingLevel Level);
	UFUNCTION(BlueprintCallable, Category = Ability)
		void QuitCloak();
};