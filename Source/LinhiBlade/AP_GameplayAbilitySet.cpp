// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "AP_GameplayAbilitySet.h"
#include "AbilitySystemComponent.h"

UAP_GameplayAbilitySet::UAP_GameplayAbilitySet()
{
	Abilities.Add(ESpellSlot::Ability1, nullptr);
	Abilities.Add(ESpellSlot::Ability2, nullptr);
	Abilities.Add(ESpellSlot::Ability3, nullptr);
	Abilities.Add(ESpellSlot::Ability4, nullptr);
	Abilities.Add(ESpellSlot::Ability5, nullptr);
	Abilities.Add(ESpellSlot::Ability6, nullptr);
	Abilities.Add(ESpellSlot::Ability7, nullptr);
	Abilities.Add(ESpellSlot::Ability8, nullptr);
	Abilities.Add(ESpellSlot::Ability9, nullptr);
}

ESpellSlot UAP_GameplayAbilitySet::Find(const UGameplayAbility* Ability)
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
	return ESpellSlot::Ability_Invalid;
}
