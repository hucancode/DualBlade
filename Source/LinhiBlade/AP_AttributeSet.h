// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AP_AttributeSet.generated.h"

#define ATTACK_SPEED_MAX 2000.0f
// at max attack speed, time between atk is 0.1s
#define ATTACK_COOLDOWN_MIN 0.1f
// at min attack speed, time between atk is 5.0s
#define ATTACK_COOLDOWN_MAX 5.0f

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class LINHIBLADE_API UAP_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAttributeEventDelegate, const FGameplayAttribute, Attribute, float, NewValue);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FValueChangeEventDelegate, float, NewValue);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGenericEventDelegate);
public:
	// Constructor and overrides
	UAP_AttributeSet();
	void AdjustEnergy(float NewValue);
	void AdjustVitality(float NewValue);
	void AdjustAgility(float NewValue);
	void AdjustStrength(float NewValue);
	void AdjustAllCoreValue(float NewStr, float NewAgi, float NewVit, float NewEne);
	void AdjustAllCoreValue();
	void AdjustAttribute(const FGameplayAttribute& Attribute, float& NewValue);
	// set health directly without notifying ability system nor clamp, use with caution
	void SetHealthUnsafe(float NewHealth);
	// set mana directly without notifying ability system nor clamp, use with caution
	void SetManaUnsafe(float NewMana);
public:
	UFUNCTION(BlueprintCallable)
		void PrintAllStats();
	UFUNCTION(BlueprintCallable)
		void GiveExp(float Amount);
	UFUNCTION(BlueprintPure)
		float GetExpPercent();
	UFUNCTION(BlueprintPure)
		float GetHealthPercent();
	UFUNCTION(BlueprintPure)
		float GetManaPercent();
public:
	virtual void InitFromMetaDataTable(const UDataTable* DataTable) override;
	virtual void PrintDebug() override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void OnAttributeAggregatorCreated(const FGameplayAttribute& Attribute, FAggregator* NewAggregator) const override;
	
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable)
		FAttributeEventDelegate OnAttributeChanged;
	UPROPERTY(BlueprintAssignable)
		FValueChangeEventDelegate OnHealthPercentChanged;
	UPROPERTY(BlueprintAssignable)
		FValueChangeEventDelegate OnManaPercentChanged;
	UPROPERTY(BlueprintAssignable)
		FGenericEventDelegate OnLevelUp; 
	UPROPERTY(BlueprintAssignable)
		FGenericEventDelegate OnDeath;

	UPROPERTY(BlueprintReadOnly, Category = "Core", ReplicatedUsing = OnRep_Strength)
		FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, Strength)
	UPROPERTY(BlueprintReadOnly, Category = "Core", ReplicatedUsing = OnRep_Agility)
		FGameplayAttributeData Agility;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, Agility)
	UPROPERTY(BlueprintReadOnly, Category = "Core", ReplicatedUsing = OnRep_Vitality)
		FGameplayAttributeData Vitality;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, Vitality)
	UPROPERTY(BlueprintReadOnly, Category = "Core", ReplicatedUsing = OnRep_Energy)
		FGameplayAttributeData Energy;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, Energy)
		
	UPROPERTY(BlueprintReadOnly, Category = "Progress", ReplicatedUsing = OnRep_Level)
		FGameplayAttributeData Level;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, Level)
	UPROPERTY(BlueprintReadOnly, Category = "Progress", ReplicatedUsing = OnRep_Experience)
		FGameplayAttributeData Experience;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, Experience)
	UPROPERTY(BlueprintReadOnly, Category = "Progress", ReplicatedUsing = OnRep_RequiredExp)
		FGameplayAttributeData RequiredExp;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, RequiredExp)
	UPROPERTY(BlueprintReadOnly, Category = "Progress", ReplicatedUsing = OnRep_BountyExp)
		FGameplayAttributeData BountyExp;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, BountyExp)
	UPROPERTY(BlueprintReadOnly, Category = "Progress", ReplicatedUsing = OnRep_AbilityPoint)
		FGameplayAttributeData AbilityPoint;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, AbilityPoint)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
		float AbilityPointGrowRate;
		UPROPERTY(BlueprintReadOnly, Category = "Progress", ReplicatedUsing = OnRep_StatPoint)
		FGameplayAttributeData StatPoint;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, StatPoint)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
		float StatPointGrowRate;
	UPROPERTY(BlueprintReadOnly, Category = "Progress", ReplicatedUsing = OnRep_DeathTime)
		FGameplayAttributeData DeathTime;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, DeathTime)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
		float DeathTimeGrowRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Strength")
		float PhysicalPowerGrowRate;
		/** AttackPower of the attacker is multiplied by the base Damage to reduce health, so 1.0 means no bonus */
	UPROPERTY(BlueprintReadOnly, Category = "Strength", ReplicatedUsing = OnRep_PhysicalPower)
		FGameplayAttributeData PhysicalPower;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, PhysicalPower)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Strength")
		float AttackSpeedGrowRate;
	UPROPERTY(BlueprintReadOnly, Category = "Strength")
		float AttackSpeedInSecond;
		/** AttackPower of the attacker is multiplied by the base Damage to reduce health, so 1.0 means no bonus */
	UPROPERTY(BlueprintReadOnly, Category = "Strength", ReplicatedUsing = OnRep_AttackSpeed)
		FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, AttackSpeed)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Agility")
		float EvasionGrowRate;
		/** AttackPower of the attacker is multiplied by the base Damage to reduce health, so 1.0 means no bonus */
	UPROPERTY(BlueprintReadOnly, Category = "Agility", ReplicatedUsing = OnRep_Evasion)
		FGameplayAttributeData Evasion;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, Evasion)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Agility")
		float MoveSpeedGrowRate;
		/** MoveSpeed affects how fast characters can move */
	UPROPERTY(BlueprintReadOnly, Category = "Agility", ReplicatedUsing = OnRep_MoveSpeed)
		FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, MoveSpeed)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Agility")
		float TurnRateGrowRate;
	UPROPERTY(BlueprintReadOnly, Category = "Agility", ReplicatedUsing = OnRep_TurnRate)
		FGameplayAttributeData TurnRate;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, TurnRate)

	UPROPERTY(BlueprintReadOnly, Category = "Vitality", ReplicatedUsing = OnRep_Health)
		FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, Health)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitality")
		float HealthRegenGrowRate;
	UPROPERTY(BlueprintReadOnly, Category = "Vitality", ReplicatedUsing = OnRep_HealthRegen)
		FGameplayAttributeData HealthRegen;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, HealthRegen)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitality")
		float MaxHealthGrowRate;
	UPROPERTY(BlueprintReadOnly, Category = "Vitality", ReplicatedUsing = OnRep_MaxHealth)
		FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, MaxHealth)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy")
		float MagicalPowerGrowRate;
		/** AttackPower of the attacker is multiplied by the base Damage to reduce health, so 1.0 means no bonus */
	UPROPERTY(BlueprintReadOnly, Category = "Energy", ReplicatedUsing = OnRep_MagicalPower)
		FGameplayAttributeData MagicalPower;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, MagicalPower)
		/** Current Mana, used to execute special abilities. Capped by MaxMana */
	UPROPERTY(BlueprintReadOnly, Category = "Energy", ReplicatedUsing = OnRep_Mana)
		FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, Mana)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy")
		float ManaRegenGrowRate;
		/** Current Mana, used to execute special abilities. Capped by MaxMana */
	UPROPERTY(BlueprintReadOnly, Category = "Energy", ReplicatedUsing = OnRep_ManaRegen)
		FGameplayAttributeData ManaRegen;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, ManaRegen)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy")
		float MaxManaGrowRate;
		/** MaxMana is its own attribute, since GameplayEffects may modify it */
	UPROPERTY(BlueprintReadOnly, Category = "Energy", ReplicatedUsing = OnRep_MaxMana)
		FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, MaxMana)
		
		/** Base Damage is divided by DefensePower to get actual damage done, so 1.0 means no bonus */
	UPROPERTY(BlueprintReadOnly, Category = "Items", ReplicatedUsing = OnRep_Armor)
		FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UAP_AttributeSet, Armor)
	
protected:
	// These OnRep functions exist to make sure that the ability system internal representations are synchronized properly during replication
	UFUNCTION()
		virtual void OnRep_Strength();
	UFUNCTION()
		virtual void OnRep_Agility();
	UFUNCTION()
		virtual void OnRep_Vitality();
	UFUNCTION()
		virtual void OnRep_Energy();

	UFUNCTION()
		virtual void OnRep_Level();
	UFUNCTION()
		virtual void OnRep_Experience();
	UFUNCTION()
		virtual void OnRep_RequiredExp();
	UFUNCTION()
		virtual void OnRep_BountyExp();
	UFUNCTION()
		virtual void OnRep_AbilityPoint();
	UFUNCTION()
		virtual void OnRep_StatPoint();
	UFUNCTION()
		virtual void OnRep_DeathTime();

	UFUNCTION()
		virtual void OnRep_PhysicalPower();
	UFUNCTION()
		virtual void OnRep_AttackSpeed();

	UFUNCTION()
		virtual void OnRep_Evasion();
	UFUNCTION()
		virtual void OnRep_MoveSpeed();
	UFUNCTION()
		virtual void OnRep_TurnRate();

	UFUNCTION()
		virtual void OnRep_Health();
	UFUNCTION()
		virtual void OnRep_HealthRegen();
	UFUNCTION()
		virtual void OnRep_MaxHealth();

	UFUNCTION()
		virtual void OnRep_MagicalPower();
	UFUNCTION()
		virtual void OnRep_Mana();
	UFUNCTION()
		virtual void OnRep_ManaRegen();
	UFUNCTION()
		virtual void OnRep_MaxMana();

	UFUNCTION()
		virtual void OnRep_Armor();

};
