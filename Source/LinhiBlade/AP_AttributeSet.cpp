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
	, Level(1.0f)
	, Experience(0.0f)
	, RequiredExp(1.0f)
	, AbilityPoint(1.0f)
	, AbilityPointGrowRate(1.0f)
	, StatPoint(1.0f)
	, StatPointGrowRate(5.0f)
	, DeathTime(10.0f)
	, DeathTimeGrowRate(1.0f)
	, PhysicalPowerGrowRate(1.0f)
	, PhysicalPower(1.0f)
	, AttackSpeedGrowRate(1.0f)
	, AttackSpeedInSecond(ATTACK_SPEED_SECOND_MAX)
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
void UAP_AttributeSet::OnRep_DeathTime()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, DeathTime);
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

void UAP_AttributeSet::OnRep_Money()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, Money);
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

void UAP_AttributeSet::OnRep_AbilityPoint()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, AbilityPoint);
}

void UAP_AttributeSet::OnRep_StatPoint()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAP_AttributeSet, StatPoint);
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
	UE_LOG(LogTemp, Warning, TEXT("UAP_AttributeSet::PreAttributeChange %s %f"), *Attribute.GetName(), NewValue);

	UAbilitySystemComponent* AC = GetOwningAbilitySystemComponent();
	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
		AAP_Hero* Owner = Cast<AAP_Hero>(AC->GetOwner());
		if (Owner)
		{
			Owner->HandleHealthChanged(GetHealth());
		}
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		AdjustAttributeForMaxChange(Mana, MaxMana, NewValue, GetManaAttribute());
		AAP_Hero* Owner = Cast<AAP_Hero>(AC->GetOwner());
		if (Owner)
		{
			Owner->HandleHealthChanged(GetMana());
		}
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
		float NewTurnRate = NewValue * TurnRateGrowRate;
		AC->ApplyModToAttribute(
			GetEvasionAttribute(), EGameplayModOp::Override, NewEvasion);
		AC->ApplyModToAttribute(
			GetMoveSpeedAttribute(), EGameplayModOp::Override, NewMoveSpeed);
		AC->ApplyModToAttribute(
			GetTurnRateAttribute(), EGameplayModOp::Override, NewTurnRate);
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
	else if (Attribute == GetMoveSpeedAttribute())
	{
		AAP_Hero* Owner = Cast<AAP_Hero>(AC->GetOwner());
		if (Owner)
		{
			Owner->HandleMoveSpeedChanged(NewValue);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UAP_AttributeSet::PreAttributeChange owner null, unbelievable"));
		}
	}
	else if (Attribute == GetTurnRateAttribute())
	{
		AAP_Hero* Owner = Cast<AAP_Hero>(AC->GetOwner());
		if (Owner->IsValidLowLevel())
		{
			Owner->HandleTurnRateChanged(NewValue);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UAP_AttributeSet::PreAttributeChange owner null, unbelievable"));
		}
	}
	else if (Attribute == GetHealthAttribute())
	{
		AAP_Hero* Owner = Cast<AAP_Hero>(AC->GetOwner());
		if (Owner->IsValidLowLevel())
		{
			Owner->HandleHealthChanged(NewValue);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UAP_AttributeSet::PreAttributeChange owner null, unbelievable"));
		}
	}
	else if (Attribute == GetManaAttribute())
	{
		AAP_Hero* Owner = Cast<AAP_Hero>(AC->GetOwner());
		if (Owner->IsValidLowLevel())
		{
			Owner->HandleManaChanged(NewValue);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UAP_AttributeSet::PreAttributeChange owner null, unbelievable"));
		}
	}
	else if (Attribute == GetExperienceAttribute())
	{
		check(GetRequiredExp() > 0);
		if(NewValue > GetRequiredExp())
		{
			SetExperience(0.0f);
			SetLevel(GetLevel() + 1.0f);
		}
	}
	else if (Attribute == GetLevelAttribute())
	{
		float delta = NewValue - GetLevel();
		// linear grow rate
		float NewAbilityPoint = GetAbilityPoint() + AbilityPointGrowRate * delta;
		SetAbilityPoint(NewAbilityPoint);
		float NewStatPoint = GetStatPoint() + StatPointGrowRate * delta;
		SetStatPoint(NewStatPoint);
		float NewDeathTime = GetDeathTime() + DeathTimeGrowRate * delta;
		SetDeathTime(NewDeathTime);
	}
	else if (Attribute == GetAbilityPointAttribute())
	{
		// notify UI
		AAP_Hero* Owner = Cast<AAP_Hero>(AC->GetOwner());
		if (Owner->IsValidLowLevel())
		{
			//Owner->HandleManaChanged(NewValue);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UAP_AttributeSet::PreAttributeChange owner null, unbelievable"));
		}
	}
	else if (Attribute == GetStatPointAttribute())
	{
		// notify UI
		AAP_Hero* Owner = Cast<AAP_Hero>(AC->GetOwner());
		if (Owner->IsValidLowLevel())
		{
			//Owner->HandleManaChanged(NewValue);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UAP_AttributeSet::PreAttributeChange owner null, unbelievable"));
		}
	}
	else if (Attribute == GetMoneyAttribute())
	{
		// notify UI
		AAP_Hero* Owner = Cast<AAP_Hero>(AC->GetOwner());
		if (Owner->IsValidLowLevel())
		{
			//Owner->HandleManaChanged(NewValue);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UAP_AttributeSet::PreAttributeChange owner null, unbelievable"));
		}
	}
}

void UAP_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("UAP_AttributeSet::PostGameplayEffectExecute, effect duration = %f"), Data.EffectSpec.Duration);
	Super::PostGameplayEffectExecute(Data);
	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();

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
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		// Clamp mana
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
	}
}
