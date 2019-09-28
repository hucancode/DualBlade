// Fill out your copyright notice in the Description page of Project Settings.

#include "AP_AttributeSet.h"
#include "GameplayEffect.h"
#include "EngineMinimal.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "AP_Hero.h"


const float UAP_AttributeSet::ATTACK_SPEED_MAX = 2000.0f;
const float UAP_AttributeSet::ATTACK_SPEED_SECOND_MIN = 0.1f;
const float UAP_AttributeSet::ATTACK_SPEED_SECOND_MAX = 5.0f;

UAP_AttributeSet::UAP_AttributeSet()
	: Strength(1.0f)
	, Agility(1.0f)
	, Vitality(1.0f)
	, Energy(1.0f)
	, PhysicalPowerGrowRate(1.0f)
	, PhysicalPower(1.0f)
	, AttackSpeedGrowRate(1.0f)
	, AttackSpeedInSecond(ATTACK_SPEED_SECOND_MAX)
	, AttackSpeed(1.0f)
	, EvasionGrowRate(0.1f)
	, Evasion(1.0f)
	, MoveSpeedGrowRate(1.0f)
	, MoveSpeed(1.0f)
	, Health(1.0f)
	, HealthRegenGrowRate(1.0f)
	, HealthRegen(1.0f)
	, MaxHealthGrowRate(1.0f)
	, MaxHealth(1.0f)
	, MagicalPowerGrowRate(1.0f)
	, MagicalPower(1.0f)
	, Mana(1.0f)
	, ManaRegenGrowRate(1.0f)
	, ManaRegen(1.0f)
	, MaxManaGrowRate(1.0f)
	, MaxMana(1.f)
	, Armor(1.0f)
{
}

void UAP_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAP_AttributeSet, Strength);
	DOREPLIFETIME(UAP_AttributeSet, Agility);
	DOREPLIFETIME(UAP_AttributeSet, Vitality);
	DOREPLIFETIME(UAP_AttributeSet, Energy);

	DOREPLIFETIME(UAP_AttributeSet, PhysicalPower);
	DOREPLIFETIME(UAP_AttributeSet, AttackSpeed);

	DOREPLIFETIME(UAP_AttributeSet, Evasion);
	DOREPLIFETIME(UAP_AttributeSet, MoveSpeed);

	DOREPLIFETIME(UAP_AttributeSet, Health);
	DOREPLIFETIME(UAP_AttributeSet, HealthRegen);
	DOREPLIFETIME(UAP_AttributeSet, MaxHealth);

	DOREPLIFETIME(UAP_AttributeSet, Mana);
	DOREPLIFETIME(UAP_AttributeSet, ManaRegen);
	DOREPLIFETIME(UAP_AttributeSet, MaxMana);
	DOREPLIFETIME(UAP_AttributeSet, MagicalPower);

	DOREPLIFETIME(UAP_AttributeSet, Armor);
}

void UAP_AttributeSet::OnRep_Health()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, Health);
}

void UAP_AttributeSet::OnRep_HealthRegen()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, HealthRegen);
}

void UAP_AttributeSet::OnRep_MaxHealth()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, MaxHealth);
}

void UAP_AttributeSet::OnRep_MagicalPower()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, MagicalPower);
}

void UAP_AttributeSet::OnRep_Mana()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, Mana);
}

void UAP_AttributeSet::OnRep_ManaRegen()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, ManaRegen);
}

void UAP_AttributeSet::OnRep_MaxMana()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, MaxMana);
}

void UAP_AttributeSet::OnRep_Armor()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, Armor);
}

void UAP_AttributeSet::OnRep_Evasion()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, Evasion);
}

void UAP_AttributeSet::OnRep_MoveSpeed()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, MoveSpeed);
}

void UAP_AttributeSet::OnRep_Strength()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, Strength);
}

void UAP_AttributeSet::OnRep_Agility()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, Agility);
}

void UAP_AttributeSet::OnRep_Vitality()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, Vitality);
}

void UAP_AttributeSet::OnRep_Energy()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, Energy);
}

void UAP_AttributeSet::OnRep_PhysicalPower()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, PhysicalPower);
}

void UAP_AttributeSet::OnRep_AttackSpeed()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, AttackSpeed);
}

void UAP_AttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UAP_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// This is called whenever attributes change, so for max health/mana we want to scale the current totals to match
	Super::PreAttributeChange(Attribute, NewValue);

	UAbilitySystemComponent* AC = GetOwningAbilitySystemComponent();
	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		AdjustAttributeForMaxChange(Mana, MaxMana, NewValue, GetManaAttribute());
	}
	else if (Attribute == GetStrengthAttribute())
	{
		float NewPhysicalPower = NewValue * PhysicalPowerGrowRate;
		float NewAttackSpeed = NewValue * AttackSpeedGrowRate;
		AC->ApplyModToAttribute(
			GetPhysicalPowerAttribute(), EGameplayModOp::Override, NewPhysicalPower);
		AC->ApplyModToAttribute(
			GetAttackSpeedAttribute(), EGameplayModOp::Override, NewAttackSpeed);
	}
	else if (Attribute == GetAgilityAttribute())
	{
		float NewEvasion = NewValue * EvasionGrowRate;
		float NewMoveSpeed = NewValue * MoveSpeedGrowRate;
		AC->ApplyModToAttribute(
			GetEvasionAttribute(), EGameplayModOp::Override, NewEvasion);
		AC->ApplyModToAttribute(
			GetMoveSpeedAttribute(), EGameplayModOp::Override, NewMoveSpeed);
	}
	else if (Attribute == GetVitalityAttribute())
	{
		float NewHealthRegen = NewValue * HealthRegenGrowRate;
		float NewMaxHealth = NewValue * MaxHealthGrowRate;
		AC->ApplyModToAttribute(
			GetHealthRegenAttribute(), EGameplayModOp::Override, NewHealthRegen);
		AC->ApplyModToAttribute(
			GetMaxHealthAttribute(), EGameplayModOp::Override, NewMaxHealth);
	}
	else if (Attribute == GetEnergyAttribute())
	{
		float NewMagicalPower = NewValue * MagicalPowerGrowRate;
		float NewManaRegen = NewValue * ManaRegenGrowRate;
		float NewMaxMana = NewValue * MaxManaGrowRate;
		AC->ApplyModToAttribute(
			GetMagicalPowerAttribute(), EGameplayModOp::Override, NewMagicalPower);
		AC->ApplyModToAttribute(
			GetManaRegenAttribute(), EGameplayModOp::Override, NewManaRegen);
		AC->ApplyModToAttribute(
			GetMaxManaAttribute(), EGameplayModOp::Override, NewMaxMana);
	}
	else if (Attribute == GetAttackSpeedAttribute())
	{
		AttackSpeedInSecond = FMath::Lerp(ATTACK_SPEED_SECOND_MIN, ATTACK_SPEED_SECOND_MAX,
			FMath::Clamp(NewValue/ATTACK_SPEED_MAX, 0.0f, 1.0f));
	}
}

void UAP_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("PostGameplayEffectExecute"));
	Super::PostGameplayEffectExecute(Data);
	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	// Compute the delta between old and new, if it is available
	float DeltaValue = 0;
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		// If this was additive, store the raw delta value to be passed along later
		DeltaValue = Data.EvaluatedData.Magnitude;
	}

	// Get the Target actor, which should be our owner
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	AAP_Hero* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<AAP_Hero>(TargetActor);
	}
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Handle other health changes such as from healing or direct modifiers
		// First clamp it
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));

		if (TargetCharacter)
		{
			// Call for all health changes
			TargetCharacter->HandleHealthChanged(DeltaValue, SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		// Clamp mana
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));

		if (TargetCharacter)
		{
			// Call for all mana changes
			TargetCharacter->HandleManaChanged(DeltaValue, SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetMoveSpeedAttribute())
	{
		if (TargetCharacter)
		{
			// Call for all movespeed changes
			TargetCharacter->HandleMoveSpeedChanged(DeltaValue, SourceTags);
		}
	}
}
