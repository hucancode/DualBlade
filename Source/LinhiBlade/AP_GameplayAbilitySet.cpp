// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "AP_GameplayAbilitySet.h"
#include "AbilitySystemComponent.h"

UAP_GameplayAbilitySet::UAP_GameplayAbilitySet()
{
	Abilities.Add(EAbilitySlot::Ability1, nullptr);
	Abilities.Add(EAbilitySlot::Ability2, nullptr);
	Abilities.Add(EAbilitySlot::Ability3, nullptr);
	Abilities.Add(EAbilitySlot::Ability4, nullptr);
	Abilities.Add(EAbilitySlot::Ability5, nullptr);
	Abilities.Add(EAbilitySlot::Ability6, nullptr);
	Abilities.Add(EAbilitySlot::Ability7, nullptr);
	Abilities.Add(EAbilitySlot::Ability8, nullptr);
	Abilities.Add(EAbilitySlot::Ability9, nullptr);
}

EAbilitySlot UAP_GameplayAbilitySet::Find(const UGameplayAbility* Ability)
{
	for(auto item : Abilities)
	{
		if (!item.Value)
		{
			continue;
		}
		if (item.Value->GetClass() == Ability->GetClass())
		{
			return item.Key;
		}

	}
	return EAbilitySlot::Ability_Invalid;
}

TSubclassOf<UGameplayAbility> UAP_GameplayAbilitySet::Find(EAbilitySlot Slot)
{
	return Abilities.FindOrAdd(Slot);
}
