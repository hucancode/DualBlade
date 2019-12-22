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
	PassiveOnEvent = 1,
	Toggle = 2,
	Active = 3,
	ActiveAutocast = 4
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
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

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
public:
	UFUNCTION(BlueprintImplementableEvent, Category = Ability)
		void OnOwnerMoved(float Magnitude);
	bool HasBlueprintOnOwnerMoved;
	UFUNCTION(BlueprintImplementableEvent, Category = Ability)
		void OnOwnerDamaged(float Magnitude, UAbilitySystemComponent* Instigator);
	bool HasBlueprintOnOwnerDamaged;
	UFUNCTION(BlueprintImplementableEvent, Category = Ability)
		void OnOwnerDamageDealt(float Magnitude, UAbilitySystemComponent* Target);
	bool HasBlueprintOnOwnerDamageDealt;
	UFUNCTION(BlueprintImplementableEvent, Category = Ability)
		void OnOwnerAttributeChanged(FGameplayAttribute Attribute, float NewValue, class UAP_AttributeSet* AllAttribute);
	bool HasBlueprintOnOwnerAttributeChanged;
};