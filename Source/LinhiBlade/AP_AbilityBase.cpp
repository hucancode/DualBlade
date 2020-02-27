// Fill out your copyright notice in the Description page of Project Settings.


#include "AP_AbilityBase.h"
#include "AP_AttributeSet.h"
#include "AbilitySystemInterface.h"
#include "AP_AttributeChangeData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "AP_FightUnit.h"

#ifndef UE_LOG_FAST
#define UE_LOG_FAST(Format, ...) UE_LOG(LogTemp, Display, Format, ##__VA_ARGS__)
#endif

UAP_AbilityBase::UAP_AbilityBase()
{
	// this could lead to countless strange behaviors, only turn this on if you really need to
	bServerRespectsRemoteAbilityCancellation = false;
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
		UE_LOG_FAST(TEXT("ShouldAbilityRespondToEvent, return false because parent call return false"));
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
				UE_LOG_FAST(TEXT("ShouldAbilityRespondToEvent, return false supplied object not match, other(%s) - me(%s)"), *other->GetName(), *me->GetName());
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

FGameplayAbilityTargetingLocationInfo UAP_AbilityBase::FrontOfOwner(float Distance)
{
	auto actor = GetOwningActorFromActorInfo();
	FGameplayAbilityTargetingLocationInfo ret;
	if (!actor)
	{
		return ret;
	}
	ret.SourceAbility = this;
	ret.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	ret.LiteralTransform = actor->GetActorTransform();
	ret.LiteralTransform.SetLocation(actor->GetActorLocation() + 
		actor->GetActorForwardVector() * 
		Distance);
	return ret;
}

float UAP_AbilityBase::OwnerRotationZ()
{
	auto actor = GetOwningActorFromActorInfo();
	if (!actor)
	{
		return 0.0f;
	}
	return actor->GetActorRotation().Yaw;
}
FGameplayAbilityTargetingLocationInfo UAP_AbilityBase::FrontOfOwnerTilted(float Distance, float Angle)
{
	auto actor = GetOwningActorFromActorInfo();
	FGameplayAbilityTargetingLocationInfo ret;
	if (!actor)
	{
		return ret;
	}
	ret.SourceAbility = this;
	ret.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	ret.LiteralTransform = actor->GetActorTransform();
	ret.LiteralTransform.SetRotation(FQuat(actor->GetActorRotation())*FQuat(FRotator(0.0f, Angle, 0.0f)));
	ret.LiteralTransform.SetLocation(actor->GetActorLocation() + 
		actor->GetActorForwardVector() * 
		Distance);
	
	return ret;
}

FGameplayTargetDataFilterHandle UAP_AbilityBase::FilterSelfOut()
{
	auto handle = FGameplayTargetDataFilterHandle();
	handle.Filter = MakeShared<FGameplayTargetDataFilter>();
	handle.Filter->SelfFilter = ETargetDataFilterSelf::TDFS_NoSelf;
	handle.Filter->SelfActor = GetOwningActorFromActorInfo();
	return handle;
}

ECollisionChannel UAP_AbilityBase::GetEnemyCollisionChannel()
{
	return ECollisionChannel();
}

void UAP_AbilityBase::SetOwnerMovementMode(EMovementMode NewMode)
{
	auto actor = GetOwningActorFromActorInfo();
	if (!actor)
	{
		return;
	}
	if (!actor->HasAuthority())
	{
		return;
	}
	auto character = Cast<ACharacter>(GetOwningActorFromActorInfo());
	if (!character)
	{
		return;
	}
	auto movement = Cast<UCharacterMovementComponent>(character->GetMovementComponent());
	if (!movement)
	{
		return;
	}
	movement->SetMovementMode(NewMode);
}

void UAP_AbilityBase::SetOwnerMovementWalk()
{
	SetOwnerMovementMode(EMovementMode::MOVE_Walking);
}

void UAP_AbilityBase::SetOwnerMovementFly()
{
	SetOwnerMovementMode(EMovementMode::MOVE_Flying);
}

bool UAP_AbilityBase::IsOwnerMovingFoward(float AngleThreshold)
{
	auto pawn = Cast<APawn>(GetOwningActorFromActorInfo());
	if (!pawn)
	{
		return false;
	}
	if (pawn->GetLastMovementInputVector().IsZero())
	{
		return false;
	}
	float angle = FMath::Acos(FVector::DotProduct(pawn->GetLastMovementInputVector(), pawn->GetActorForwardVector()));
	return angle <= AngleThreshold;
}

bool UAP_AbilityBase::IsOwnerInAir()
{
	auto character = Cast<ACharacter>(GetOwningActorFromActorInfo());
	if (!character)
	{
		return false;
	}
	return character->GetCharacterMovement()->IsFalling();
}

bool UAP_AbilityBase::IsBuffApplied(TSubclassOf<UGameplayEffect> BuffClass)
{
	auto asc = GetAbilitySystemComponentFromActorInfo();
	if (!asc)
	{
		return false;
	}
	return asc->GetGameplayEffectCount(BuffClass, nullptr) > 0;
}

void UAP_AbilityBase::ClearBuff(TSubclassOf<UGameplayEffect> BuffClass)
{
	auto asc = GetAbilitySystemComponentFromActorInfo();
	if (!asc)
	{
		return;
	}
	asc->RemoveActiveGameplayEffectBySourceEffect(BuffClass, nullptr);
}

void UAP_AbilityBase::EnterCloak(ECloakingLevel Level)
{
	auto unit = Cast<AAP_FightUnit>(GetOwningActorFromActorInfo());
	if (!unit)
	{
		return;
	}
	if (!unit->TeamAgent)
	{
		return;
	}
	unit->TeamAgent->EnterCloak(Level);
}

void UAP_AbilityBase::QuitCloak()
{
	auto unit = Cast<AAP_FightUnit>(GetOwningActorFromActorInfo());
	if (!unit)
	{
		return;
	}
	if (!unit->TeamAgent)
	{
		return;
	}
	unit->TeamAgent->QuitCloak();
}

