// Fill out your copyright notice in the Description page of Project Settings.


#include "AP_AbilityBase.h"
#include "AP_AttributeSet.h"
#include "AbilitySystemInterface.h"
#include "AP_AttributeChangeData.h"

#ifndef UE_LOG_FAST
#define UE_LOG_FAST(Format, ...) UE_LOG(LogTemp, Display, Format, ##__VA_ARGS__)
#endif

UAP_AbilityBase::UAP_AbilityBase()
{
	{
		FAbilityTriggerData data;
		data.TriggerTag = FGameplayTag::RequestGameplayTag("Combat.Trigger_Ability_By_Controller");
		AbilityTriggers.Add(data);
	}
	auto ImplementedInBlueprint = [](const UFunction* Func) -> bool
	{
		return Func && ensure(Func->GetOuter())
			&& (Func->GetOuter()->IsA(UBlueprintGeneratedClass::StaticClass()) || Func->GetOuter()->IsA(UDynamicClass::StaticClass()));
	};
	{
		static FName FuncName = FName(TEXT("OnOwnerMoved"));
		HasBlueprintOnOwnerMoved = ImplementedInBlueprint(GetClass()->FindFunctionByName(FuncName));
	}
	{
		static FName FuncName = FName(TEXT("OnOwnerDamaged"));
		HasBlueprintOnOwnerDamaged = ImplementedInBlueprint(GetClass()->FindFunctionByName(FuncName));
	}
	{
		static FName FuncName = FName(TEXT("OnOwnerDamageDealt"));
		HasBlueprintOnOwnerDamageDealt = ImplementedInBlueprint(GetClass()->FindFunctionByName(FuncName));
	}
	{
		static FName FuncName = FName(TEXT("OnOwnerAttributeChanged"));
		HasBlueprintOnOwnerAttributeChanged = ImplementedInBlueprint(GetClass()->FindFunctionByName(FuncName));
	}
}
UAP_AbilityBase::~UAP_AbilityBase()
{

}

bool UAP_AbilityBase::ShouldAbilityRespondToEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData* Payload) const
{
	bool ret = Super::ShouldAbilityRespondToEvent(ActorInfo, Payload);
	if (!ret)
	{
		return false;
	}
	{
		auto tag = FGameplayTag::RequestGameplayTag("Combat.Trigger_Ability_By_Controller");
		bool tag_matched = Payload->EventTag.MatchesTag(tag);
		if (tag_matched)
		{
			if (!Payload->OptionalObject)
			{
				UE_LOG_FAST(TEXT("ShouldAbilityRespondToEvent, return false supplied object null"));
				return false;
			}
			auto other = Payload->OptionalObject->GetClass();
			auto me = GetClass();
			if (other != me)
			{
				UE_LOG_FAST(TEXT("ShouldAbilityRespondToEvent, return false supplied object not match"));
				return false;
			}
			UE_LOG_FAST(TEXT("ShouldAbilityRespondToEvent, return true"));
			return true;
		}
	}
	{
		auto tag = FGameplayTag::RequestGameplayTag("Combat.Trigger_Ability_On_Moved");
		bool tag_matched = Payload->EventTag.MatchesTag(tag);
		if (tag_matched && !HasBlueprintOnOwnerMoved)
		{
			UE_LOG_FAST(TEXT("ShouldAbilityRespondToEvent, return false blueprint not implemented"));
			return false;
		}
	}
	{
		auto tag = FGameplayTag::RequestGameplayTag("Combat.Trigger_Ability_On_Damaged");
		bool tag_matched = Payload->EventTag.MatchesTag(tag);
		if (tag_matched && !HasBlueprintOnOwnerDamaged)
		{
			UE_LOG_FAST(TEXT("ShouldAbilityRespondToEvent, return false blueprint not implemented"));
			return false;
		}
	}
	{
		auto tag = FGameplayTag::RequestGameplayTag("Combat.Trigger_Ability_On_Damage_Dealt");
		bool tag_matched = Payload->EventTag.MatchesTag(tag);
		if (tag_matched && !HasBlueprintOnOwnerDamageDealt)
		{
			UE_LOG_FAST(TEXT("ShouldAbilityRespondToEvent, return false blueprint not implemented"));
			return false;
		}
	}
	{
		auto tag = FGameplayTag::RequestGameplayTag("Combat.Trigger_Ability_On_Attribute_Changed");
		bool tag_matched = Payload->EventTag.MatchesTag(tag);
		if (tag_matched && !HasBlueprintOnOwnerAttributeChanged)
		{
			UE_LOG_FAST(TEXT("ShouldAbilityRespondToEvent, return false blueprint not implemented"));
			return false;
		}
	}
	UE_LOG_FAST(TEXT("ShouldAbilityRespondToEvent, return true"));
	return true;
}

void UAP_AbilityBase::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG_FAST(TEXT("ActivateAbility, preparing to activate ability"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!TriggerEventData)
	{
		return;
	}
	{
		auto tag = FGameplayTag::RequestGameplayTag("Combat.Trigger_Ability_On_Moved");
		bool tag_matched = TriggerEventData->EventTag.MatchesTag(tag);
		if (tag_matched)
		{
			OnOwnerMoved(TriggerEventData->EventMagnitude);
		}
	}
	{
		auto tag = FGameplayTag::RequestGameplayTag("Combat.Trigger_Ability_On_Damaged");
		bool tag_matched = TriggerEventData->EventTag.MatchesTag(tag);
		if (tag_matched)
		{
			auto instigator = Cast<IAbilitySystemInterface>(TriggerEventData->Instigator);
			if (instigator)
			{
				auto asc = instigator->GetAbilitySystemComponent();
				OnOwnerDamaged(TriggerEventData->EventMagnitude, asc);
			}
		}
	}
	{
		auto tag = FGameplayTag::RequestGameplayTag("Combat.Trigger_Ability_On_Damage_Dealt");
		bool tag_matched = TriggerEventData->EventTag.MatchesTag(tag);
		if (tag_matched)
		{
			auto target = Cast<IAbilitySystemInterface>(TriggerEventData->Target);
			if (target)
			{
				auto asc = target->GetAbilitySystemComponent();
				OnOwnerDamageDealt(TriggerEventData->EventMagnitude, asc);
			}
		}
	}
	{
		auto tag = FGameplayTag::RequestGameplayTag("Combat.Trigger_Ability_On_Attribute_Changed");
		bool tag_matched = TriggerEventData->EventTag.MatchesTag(tag);
		if (tag_matched)
		{
			auto owner = Cast<IAbilitySystemInterface>(TriggerEventData->Instigator);
			if (owner)
			{
				auto asc = owner->GetAbilitySystemComponent();
				auto attribute = Cast<UAP_AttributeChangeData>(TriggerEventData->OptionalObject);
				if (attribute)
				{
					OnOwnerAttributeChanged(attribute->Attribute, attribute->NewValue, attribute->AllAttribute);
				}
			}
		}
	}
}

ECollisionChannel UAP_AbilityBase::GetTraceChannel()
{
	// Those channel are subject to change
	// See DefaultEngine.ini for lastest update
	auto ret = ECollisionChannel();
	switch (TargetingPolicy)
	{
	case ETargetingPolicy::None:
		break;
	case ETargetingPolicy::Ground:
		ret = ECollisionChannel::ECC_GameTraceChannel3; //Floor
		break;
	case ETargetingPolicy::UnitAll:
		ret = ECollisionChannel::ECC_GameTraceChannel11; //Unit_All
		break;
	case ETargetingPolicy::UnitAlly:
		// need to calculate this on run time
		ret = ECollisionChannel::ECC_GameTraceChannel8; //Unit_Team1
		break;
	case ETargetingPolicy::UnitAllyExcludeSelf:
		// need to calculate this on run time
		ret = ECollisionChannel::ECC_GameTraceChannel8; //Unit_Team1
		break;
	case ETargetingPolicy::UnitEnemy:
		// need to calculate this on run time
		ret = ECollisionChannel::ECC_GameTraceChannel9; //Unit_Team2
		break;
	default:
		break;
	}
	return ret;
}

