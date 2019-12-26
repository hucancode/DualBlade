// Fill out your copyright notice in the Description page of Project Settings.

#include "AP_AttributeSet.h"
#include "GameplayEffect.h"
#include "EngineMinimal.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UAP_AttributeSet::UAP_AttributeSet()
	: Strength(1.0f)
	, Agility(1.0f)
	, Vitality(1.0f)
	, Energy(1.0f)
	, Level(1.0f)
	, Experience(0.0f)
	, RequiredExp(1.0f)
	, BountyExp(1.0f)
	, AbilityPoint(1.0f)
	, AbilityPointGrowRate(1.0f)
	, StatPoint(1.0f)
	, StatPointGrowRate(5.0f)
	, DeathTime(10.0f)
	, DeathTimeGrowRate(1.0f)
	, PhysicalPowerGrowRate(1.0f)
	, PhysicalPower(1.0f)
	, AttackSpeedGrowRate(1.0f)
	, AttackSpeedInSecond(ATTACK_COOLDOWN_MAX)
	, AttackSpeed(1.0f)
	, EvasionGrowRate(0.1f)
	, Evasion(1.0f)
	, MoveSpeedGrowRate(1.0f)
	, MoveSpeed(1.0f)
	, TurnRateGrowRate(1.0f)
	, TurnRate(1.0f)
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
	
	DOREPLIFETIME(UAP_AttributeSet, Level);
	DOREPLIFETIME(UAP_AttributeSet, Experience);
	DOREPLIFETIME(UAP_AttributeSet, RequiredExp);
	DOREPLIFETIME(UAP_AttributeSet, BountyExp);
	DOREPLIFETIME(UAP_AttributeSet, AbilityPoint);
	DOREPLIFETIME(UAP_AttributeSet, StatPoint);
	DOREPLIFETIME(UAP_AttributeSet, DeathTime);

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
void UAP_AttributeSet::OnRep_TurnRate()
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

void UAP_AttributeSet::OnRep_Level()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, Level);
}

void UAP_AttributeSet::OnRep_Experience()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, Experience);
}

void UAP_AttributeSet::OnRep_RequiredExp()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, RequiredExp);
}

void UAP_AttributeSet::OnRep_BountyExp()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, BountyExp);
}

void UAP_AttributeSet::OnRep_AbilityPoint()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, AbilityPoint);
}

void UAP_AttributeSet::OnRep_StatPoint()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, StatPoint);
}
void UAP_AttributeSet::OnRep_DeathTime()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, DeathTime);
}

void UAP_AttributeSet::OnRep_PhysicalPower()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, PhysicalPower);
}

void UAP_AttributeSet::OnRep_AttackSpeed()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, AttackSpeed);
}

float UAP_AttributeSet::GetHealthPercent()
{
	if (GetMaxHealth() <= 0.0f)
	{
		return 0.0f;
	}
	return GetHealth() / GetMaxHealth();
}

float UAP_AttributeSet::GetManaPercent()
{
	if (GetMaxMana() <= 0.0f)
	{
		return 0.0f;
	}
	return GetMana() / GetMaxMana();
}

void UAP_AttributeSet::InitFromMetaDataTable(const UDataTable* DataTable)
{
	Super::InitFromMetaDataTable(DataTable);
	AdjustAllCoreValue();
}

void UAP_AttributeSet::PrintDebug()
{
	UE_LOG(LogTemp, Warning, TEXT("UAP_AttributeSet::PrintDebug ..."));
	for (TFieldIterator<UProperty> It(GetClass(), EFieldIteratorFlags::IncludeSuper); It; ++It)
	{
		UProperty* Property = *It;
		if (FGameplayAttribute::IsGameplayAttributeDataProperty(Property))
		{
			UStructProperty* StructProperty = Cast<UStructProperty>(Property);
			check(StructProperty);
			FGameplayAttributeData* DataPtr = StructProperty->ContainerPtrToValuePtr<FGameplayAttributeData>(this);
			check(DataPtr);
			UE_LOG(LogTemp, Warning, TEXT("%s %f(%f)"), *Property->GetName(),  DataPtr->GetBaseValue(), DataPtr->GetCurrentValue());
		}
	}
}

void UAP_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	UE_LOG(LogTemp, Warning, TEXT("UAP_AttributeSet::PreAttributeChange %s %f"), *Attribute.GetName(), NewValue);
	AdjustAttribute(Attribute, NewValue);
	OnAttributeChanged.Broadcast(Attribute, NewValue);
}

void UAP_AttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	UE_LOG(LogTemp, Warning, TEXT("UAP_AttributeSet::PreAttributeBaseChange %s %f"), *Attribute.GetName(), NewValue);
	//AdjustAttribute(Attribute, NewValue);
	//OnAttributeChanged.Broadcast(Attribute, NewValue);
}

void UAP_AttributeSet::OnAttributeAggregatorCreated(const FGameplayAttribute& Attribute, FAggregator* NewAggregator) const
{
	Super::OnAttributeAggregatorCreated(Attribute, NewAggregator);
	UE_LOG(LogTemp, Warning, TEXT("UAP_AttributeSet::OnAttributeAggregatorCreated %s"), *Attribute.GetName());
}

void UAP_AttributeSet::AdjustAttribute(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetMaxHealthAttribute())
	{
		float Delta = GetMaxHealth() - GetHealth();
		float NewHealth = FMath::Clamp(NewValue - Delta, 1.0f, NewValue);
		float new_percent = NewValue > 0.0f ? (NewHealth / NewValue) : 0.0f;
		SetHealthUnsafe(NewHealth);
		OnHealthPercentChanged.Broadcast(new_percent);
		return;
	}
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
		if (NewValue <= 0.0f)
		{
			OnDeath.Broadcast();
		}
		float max_health = GetMaxHealth();
		float new_percent = max_health > 0.0f ? (NewValue/max_health) : 0.0f;
		OnHealthPercentChanged.Broadcast(new_percent);
		return;
	}
	if (Attribute == GetMaxManaAttribute())
	{
		float Delta = GetMaxMana() - GetMana();
		float NewMana = FMath::Clamp(NewValue - Delta, 0.0f, NewValue);
		float new_percent = NewValue > 0.0f ? (NewMana / NewValue) : 0.0f;
		SetManaUnsafe(NewMana);
		OnManaPercentChanged.Broadcast(new_percent);
		return;
	}
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
		float max_mana = GetMaxMana();
		float new_percent = max_mana > 0.0f ? (NewValue / max_mana) : 0.0f;
		OnManaPercentChanged.Broadcast(new_percent);
		return;
	}
	if (Attribute == GetStrengthAttribute())
	{
		return AdjustStrength(NewValue);
	}
	if (Attribute == GetAgilityAttribute())
	{
		return AdjustAgility(NewValue);
	}
	if (Attribute == GetVitalityAttribute())
	{
		return AdjustVitality(NewValue);
	}
	if (Attribute == GetEnergyAttribute())
	{
		return AdjustEnergy(NewValue);
	}
	if (Attribute == GetAttackSpeedAttribute())
	{
		float r = NewValue / ATTACK_SPEED_MAX;
		r = FMath::Clamp(r, 0.0f, 1.0f);
		AttackSpeedInSecond = FMath::Lerp(ATTACK_COOLDOWN_MAX, ATTACK_COOLDOWN_MIN, r);
		return;
	}
	if (Attribute == GetExperienceAttribute())
	{
		check(GetRequiredExp() > 0);
		if (NewValue < GetRequiredExp())
		{
			return;
		}
		SetExperience(0.0f);
		SetLevel(GetLevel() + 1.0f);
		OnLevelUp.Broadcast();
	}
	if (Attribute == GetLevelAttribute())
	{
		float delta = NewValue - GetLevel();
		// linear grow rate
		float NewAbilityPoint = GetAbilityPoint() + AbilityPointGrowRate * delta;
		SetAbilityPoint(NewAbilityPoint);
		float NewStatPoint = GetStatPoint() + StatPointGrowRate * delta;
		SetStatPoint(NewStatPoint);
		float NewDeathTime = GetDeathTime() + DeathTimeGrowRate * delta;
		SetDeathTime(NewDeathTime);
		return;
	}
}

void UAP_AttributeSet::SetHealthUnsafe(float NewHealth)
{
	if (NewHealth <= 0.0f)
	{
		OnDeath.Broadcast();
	}
	Health.SetBaseValue(NewHealth);
	Health.SetCurrentValue(NewHealth);
	float max_health = GetMaxHealth();
	float new_percent = max_health > 0.0f ? (NewHealth / max_health) : 0.0f;
	OnHealthPercentChanged.Broadcast(new_percent);
}

void UAP_AttributeSet::SetManaUnsafe(float NewMana)
{
	Mana.SetBaseValue(NewMana);
	Mana.SetCurrentValue(NewMana);
	float max_mana = GetMaxMana();
	float new_percent = max_mana > 0.0f ? (NewMana / max_mana) : 0.0f;
	OnManaPercentChanged.Broadcast(new_percent);
}

void UAP_AttributeSet::PrintAllStats()
{
	PrintDebug();
}

void UAP_AttributeSet::GiveExp(float Amount)
{
	SetExperience(GetExperience() + Amount);
}

float UAP_AttributeSet::GetExpPercent()
{
	if (GetRequiredExp() <= 0.0f)
	{
		return 0.0f;
	}
	return GetExperience()/GetRequiredExp();
}

void UAP_AttributeSet::AdjustEnergy(float NewValue)
{
	float NewMagicalPower = NewValue * MagicalPowerGrowRate;
	float NewManaRegen = NewValue * ManaRegenGrowRate;
	float NewMaxMana = NewValue * MaxManaGrowRate;
	SetMagicalPower(NewMagicalPower);
	SetManaRegen(NewManaRegen);
	SetMaxMana(NewMaxMana);
}

void UAP_AttributeSet::AdjustVitality(float NewValue)
{
	float NewHealthRegen = NewValue * HealthRegenGrowRate;
	float NewMaxHealth = NewValue * MaxHealthGrowRate;
	SetHealthRegen(NewHealthRegen);
	SetMaxHealth(NewMaxHealth);
}

void UAP_AttributeSet::AdjustAgility(float NewValue)
{
	float NewEvasion = NewValue * EvasionGrowRate;
	float NewMoveSpeed = NewValue * MoveSpeedGrowRate;
	float NewTurnRate = NewValue * TurnRateGrowRate;
	SetEvasion(NewEvasion);
	SetMoveSpeed(NewMoveSpeed);
	SetTurnRate(NewTurnRate);
}

void UAP_AttributeSet::AdjustStrength(float NewValue)
{
	float NewPhysicalPower = NewValue * PhysicalPowerGrowRate;
	float NewAttackSpeed = NewValue * AttackSpeedGrowRate;
	SetPhysicalPower(NewPhysicalPower);
	SetAttackSpeed(NewAttackSpeed);
}

void UAP_AttributeSet::AdjustAllCoreValue(float NewStr, float NewAgi, float NewVit, float NewEne)
{
	AdjustStrength(NewStr);
	AdjustAgility(NewAgi);
	AdjustVitality(NewVit);
	AdjustEnergy(NewEne);
}

void UAP_AttributeSet::AdjustAllCoreValue()
{
	AdjustAllCoreValue(GetStrength(), 
		GetAgility(), 
		GetVitality(), 
		GetEnergy());
}

void UAP_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("UAP_AttributeSet::PostGameplayEffectExecute, effect duration = %f"), Data.EffectSpec.Duration);
	Super::PostGameplayEffectExecute(Data);
}
