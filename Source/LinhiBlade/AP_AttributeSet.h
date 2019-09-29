// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AP_AttributeSet.generated.h"


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
public:

	// Constructor and overrides
	UAP_AttributeSet();
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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
	/** Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes. (i.e. When MaxHealth increases, Health increases by an amount that maintains the same percentage as before) */
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

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
	static const float ATTACK_SPEED_MAX;
	static const float ATTACK_SPEED_SECOND_MIN;
	static const float ATTACK_SPEED_SECOND_MAX;
};
